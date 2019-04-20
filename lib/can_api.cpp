/*
 * can_api.cpp
 *
 *  Created on: Mar 5, 2019
 *      Author: HB
 */

#include "can_api.h"

#include "MKE18F16.h"

/* RX CS status and control codes */
static constexpr uint8_t CS_RX_INACTIVE 	= 0b0000;
static constexpr uint8_t CS_RX_EMPTY 		= 0b0100;
static constexpr uint8_t CS_RX_FULL 		= 0b0010;
static constexpr uint8_t CS_RX_OVERRUN 		= 0b0110;
static constexpr uint8_t CS_RX_RANSWER		= 0b1010;
static constexpr uint8_t CS_RX_BUSY 		= 0b0001;

static constexpr uint8_t CS_RX_INACTIVE_MASK 	= 0x0F;
static constexpr uint8_t CS_RX_EMPTY_MASK 		= 0x0F;
static constexpr uint8_t CS_RX_FULL_MASK 		= 0x0F;
static constexpr uint8_t CS_RX_OVERRUN_MASK 	= 0x0F;
static constexpr uint8_t CS_RX_RANSWER_MASK		= 0x0F;
static constexpr uint8_t CS_RX_BUSY_MASK 		= 0x01;

/* TX CS status and control codes */
static constexpr uint8_t CS_TX_INACTIVE 	= 0b1000;
static constexpr uint8_t CS_TX_ABORT 		= 0b1001;
static constexpr uint8_t CS_TX_DATAREMOTE	= 0b1100;
static constexpr uint8_t CS_TX_TANSWER		= 0b1110;

static constexpr uint8_t CS_TX_INACTIVE_MASK 	= 0x0F;
static constexpr uint8_t CS_TX_ABORT_MASK 		= 0x0F;
static constexpr uint8_t CS_TX_DATAREMOTE_MASK 	= 0x0F;
static constexpr uint8_t CS_TX_TANSWER_MASK		= 0x0F;

using namespace can;

static void can0_irq() {
	// TODO Make an NVIC driver that can pass user data, or uses se::Event.
	CAN0Description.driver->can_irq_handler();
	__DSB(); // for good luck
}

static void can1_irq() {
	// TODO Make an NVIC driver that can pass user data, or uses se::Event.
	CAN1Description.driver->can_irq_handler();
	__DSB(); // for good luck
}

can_api::can_api(can_t* obj) : obj{obj}, handler{}, interruptMask{} {
	assert(obj);
	assert(!obj->driver);

	obj->driver = this;
}

can_api::~can_api() {
	reset();
	NVIC_DisableIRQ(obj->irqn);
	obj->driver = nullptr;
}

void can_api::init(const can_config_t& config) {

	/* FlexCAN initialization sequence (50.6.1) on pg 1457 of Reference */
	/* enable peripheral clock source for the CAN module */
	CLOCK_EnableClock(obj->clock);

	/* make sure the CAN module is disabled */
	enable(false);

	/* Protocol-Engine clock source selection, bit must be set
	 * when FlexCAN Module in Disable Mode. Select peripheral clock*/
	obj->base->CTRL1 = /*(kFLEXCAN_ClkSrcOsc == config->clkSrc) ?
					base->CTRL1 & ~CAN_CTRL1_CLKSRC_MASK :*/
					obj->base->CTRL1 | CAN_CTRL1_CLKSRC_MASK;



	/* enable CAN module to allow for configuration*/
	enable(true);

	/* put controller into a known state*/
	reset();

	uint32_t mcr = obj->base->MCR;
	/*
	 * Enable the individual filtering per MB and reception queue features by setting the IRMQ bit
	 * Enable the warning interrupts by setting the WRNEN bit
	 * Disable frame self reception by setting the SRXDIS bit
	 * Enable the abort mechanism by setting the AEN bit
	 * */
	mcr |= CAN_MCR_IRMQ_MASK | CAN_MCR_WRNEN_MASK | CAN_MCR_SRXDIS_MASK | CAN_MCR_AEN_MASK;

	/* enable self wake if requested */
	mcr = (config.enableSelfWake) ? mcr | CAN_MCR_SLFWAK_MASK : mcr & ~CAN_MCR_SLFWAK_MASK;

	obj->base->MCR = mcr;

	/* enable loopback if requested */
	obj->base->CTRL1 = (config.enableLoopback) ? obj->base->CTRL1 | CAN_CTRL1_LPB_MASK : obj->base->CTRL1 & ~CAN_CTRL1_LPB_MASK;

	/* Point irq vector to can_irq and enable irq*/
	// TODO NVIC vector user data
	// TODO Enable other error irqs
	if(obj->base == CAN0) {
		NVIC_SetVector(obj->irqn, (uint32_t) &can0_irq);
	} else {
		NVIC_SetVector(obj->irqn, (uint32_t) &can1_irq);
	}

	NVIC_EnableIRQ(obj->irqn);

	/* exit freeze mode. Automatically enabled after clock selection/module enabled*/
	freeze(false);

	/* config bus speed*/
	setupClock(config.baud);
}

int32_t can_api::write(const CANMessage& msg) {
	assert(!(obj->base->MCR & CAN_MCR_MDIS_MASK));

	/* find an available mb */
	uint16_t mbid = findFirstInactiveMB();
	if(mbid < 0)
		return -1;

	configTxMB(mbid, msg);

	/* don't return a handle */
	return 0;
}

int32_t can_api::read(CANMessage& msg, int32_t handle) {
	assert(!(obj->base->MCR & CAN_MCR_MDIS_MASK));
	assert(handle >= 0);

	uint16_t mbid;
	if(handle == 0) {
		/* findFirstMBWithCode will lock the MB if CS CODE is FULL */
		mbid = findFirstMBWithCode(CS_RX_FULL, CS_RX_FULL_MASK);
		handle = mbid + 1;
		if(mbid < 0)
			return -1;
	} else {
		mbid = handle - 1;
	}

	/* Read handle mb and error if mb was not full*/
	if(readMB(mbid, msg) < 0)
		return -1;

	/* mb is locked by reading CS register, and unlocked by
	 * reading free running timer or by reading cs on another mb */
	[[maybe_unused]]
	uint32_t tmr = obj->base->TIMER;

	return mbid + 1;
}

uint8_t can_api::rerror() {
	return (obj->base->ECR & CAN_ECR_RXERRCNT_MASK) >> CAN_ECR_RXERRCNT_SHIFT;
}

uint8_t can_api::terror() {
	return (obj->base->ECR & CAN_ECR_TXERRCNT_MASK) >> CAN_ECR_TXERRCNT_SHIFT;
}

int32_t can_api::filter(uint32_t id, uint32_t mask, CanFormat format, CanFrameType type, bool filterType, int32_t handle) {
	assert(!(obj->base->MCR & CAN_MCR_MDIS_MASK));
	assert(handle >= 0);

	uint16_t mbid = 0;
	// check for no handle
	if(handle == 0) {
		mbid = findFirstInactiveMB();
		handle = mbid + 1;
		if(mbid < 0)	// check for invalid handle
			return -1;
	} else {
		mbid = handle - 1;
	}

	configRxMB(mbid, id, mask, format, type, filterType, false);

	return handle;
}

bool can_api::isInUse(int32_t handle) {
	assert(handle > 0);

	uint8_t code = (uint8_t)((obj->base->MB[handle - 1].CS & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT);
	if (code == CS_RX_FULL || code == CS_RX_EMPTY || code == CS_RX_BUSY || code == CS_RX_OVERRUN
			|| code == CS_TX_DATAREMOTE)
		return true;
	return false;
}

int32_t can_api::closeFilter(int32_t handle) {
	assert(handle > 0);
	/* reset all fields in mb and disable interrupt */
	clearMB(handle - 1);
	return 0;
}

void can_api::can_irq_init(CanIrqHandlerType handler) {
	this->handler = handler;
}

void can_api::can_irq_set(CanIrqType irq, bool enabled) {
	switch(irq) {
	case CanIrqType::ARB:
	case CanIrqType::BUS:
	case CanIrqType::READY:
	case CanIrqType::WAKEUP:
		// unsupported
		break;
	// TODO: enable error and passive interrupts
	case CanIrqType::ERROR:
	case CanIrqType::OVERRUN:
	case CanIrqType::PASSIVE:
	case CanIrqType::RX:
	case CanIrqType::TX:
		if (enabled) {
			interruptMask |= 1 << (uint16_t) irq;
		} else {
			interruptMask &= ~(1 << (uint16_t) irq);
		}
		break;
	}
}

void can_api::can_irq_free() {
	handler = nullptr;
	interruptMask = 0;
}

void can_api::can_irq_handler() {

	/*
	 const uint32_t esr1_errors =
	 CAN_ESR1_TWRNINT_MASK |
	 CAN_ESR1_RWRNINT_MASK |
	 CAN_ESR1_BOFFDONEINT_MASK |
	 CAN_ESR1_WAKINT_MASK |
	 CAN_ESR1_ERRINT_MASK |
	 CAN_ESR1_BOFFINT_MASK;
	 */
	do {
		uint32_t esr = obj->base->ESR1;
		//TODO Support more errors
		//TODO Add NVIC interrupt handlers for error interrupts (in init)
		if(esr & CAN_ESR1_ERRINT_MASK) {
			// report and clear error (by writing 1)
			obj->base->ESR1 = CAN_ESR1_ERRINT_MASK;
			if(interruptMask & (uint16_t)CanIrqType::ERROR) {
				assert(handler);
				handler(CanIrqType::ERROR, this, 0);
			}
		} else {
			uint16_t mbid = 0;
			// handle mb interrupt
			/* find lowest mb that generated an interrupt */
			for (mbid = 0; mbid < FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base); mbid++)
			{
				/* check that both mask and flag are set: interrupt is triggered and enabled */
				if((obj->base->IFLAG1 & (1 << mbid)) && obj->base->IMASK1 & 1 << mbid) {
					break;
				}
			}

			uint8_t code = (obj->base->MB[mbid].CS & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT;

			switch(code) {
			case CS_RX_FULL:
				if (interruptMask & (uint16_t) CanIrqType::RX) {
					assert(handler);
					handler(CanIrqType::RX, this, mbid + 1);
				}
				break;
			case CS_TX_INACTIVE:
				if (interruptMask & (uint16_t) CanIrqType::TX) {
					assert(handler);
					handler(CanIrqType::TX, this, mbid + 1);
				}
				break;
			case CS_RX_OVERRUN:
				// RX message buffer has been overwritten with another message before the mb was serviced
				if (interruptMask & (uint16_t) CanIrqType::OVERRUN) {
					assert(handler);
					handler(CanIrqType::OVERRUN, this, mbid + 1);
				}
			}

			/* clear iflag by writing a 1 to it */
			obj->base->IFLAG1 = (1 << mbid);
		}

	} while((obj->base->IFLAG1 & obj->base->IMASK1) != 0 || (obj->base->ESR1 & CAN_ESR1_ERRINT_MASK));
}

void can_api::enable(bool enabled) {
	if (enabled) {
		/* enable CAN module. Clear MDIS bit */
		obj->base->MCR &= ~CAN_MCR_MDIS_MASK;

		/* Wait FlexCAN exit from low-power mode. */
		while (obj->base->MCR & CAN_MCR_LPMACK_MASK)
			;
	} else {
		/* disable CAN module. Set MDIS bit */
		obj->base->MCR |= CAN_MCR_MDIS_MASK;

		/* Wait FlexCAN enter low-power mode. */
		while (!(obj->base->MCR & CAN_MCR_LPMACK_MASK))
			;
	}
}

void can_api::freeze(bool enabled) {

	if(enabled) {
		/* enable and request freeze mode */
		obj->base->MCR |= CAN_MCR_FRZ_MASK;
		obj->base->MCR |= CAN_MCR_HALT_MASK;

		/* wait for freeze mode to be enabled */
		while (!(obj->base->MCR & CAN_MCR_FRZACK_MASK))
			;
	} else {
		/* clear freeze and halt bits */
		obj->base->MCR |= CAN_MCR_FRZ_MASK;
		obj->base->MCR |= CAN_MCR_HALT_MASK;

		/* wait for freeze mode disable to be acknowledged */
		while (obj->base->MCR & CAN_MCR_FRZACK_MASK)
			;
	}

}

void can_api::reset() {
	/* ensure CAN module is enabled */
	assert(obj);
	assert(!(obj->base->MCR & CAN_MCR_MDIS_MASK));

	/* Wait FlexCAN exit from low-power mode. */
	while (obj->base->MCR & CAN_MCR_LPMACK_MASK)
		;

	/* reset CAN module */
	obj->base->MCR &= CAN_MCR_SOFTRST_MASK;

	/* wait for reset to complete */
	while (obj->base->MCR & CAN_MCR_SOFTRST_MASK)
		;

	/* enable warnings, internal wakeup filter, and reset max mb number */
	obj->base->MCR |= CAN_MCR_WRNEN_MASK | CAN_MCR_WAKSRC_MASK |
		                 CAN_MCR_MAXMB(FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(0) - 1);

	/* reset CTRL1 and CTRL2*/
	/* Enable multiple samples per bit CAN_CTRL1_SMP_MASK
	 * */
	obj->base->CTRL1 = CAN_CTRL1_SMP_MASK;
	/* Set Tx Arbitration Start Delay to default value TASD, Enable remote request storing, Enable Entire Frame Arbitration Field Comparison
	 * pg 1407 */
	obj->base->CTRL2 = CAN_CTRL2_TASD(0x16) | CAN_CTRL2_RRS_MASK | CAN_CTRL2_EACEN_MASK;

	/* Clean all individual Rx Mask of Message Buffers. */
	uint16_t i;
	for (i = 0; i < FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base); i++)
	{
		obj->base->RXIMR[i] = 0x3FFFFFFF;
	}

	/* Clean Global Mask of Message Buffers. */
	obj->base->RXMGMASK = 0x3FFFFFFF;
	/* Clean Global Mask of Message Buffer 14. */
	obj->base->RX14MASK = 0x3FFFFFFF;
	/* Clean Global Mask of Message Buffer 15. */
	obj->base->RX15MASK = 0x3FFFFFFF;
	/* Clean Global Mask of Rx FIFO. */
	obj->base->RXFGMASK = 0x3FFFFFFF;

	/* Clean all Message Buffer CS fields. */
	for (i = 0; i < FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base); i++)
	{
		obj->base->MB[i].CS = 0x0;
	}
}

void can_api::setupClock(uint32_t baud) {
	/* ensure CAN module is enabled and in freeze mode*/
	assert(!(obj->base->MCR & CAN_MCR_MDIS_MASK));
	freeze(true);

	assert(baud <= 1000000);

	/* Get FlexCAN source clock frequency */
	uint32_t srcClk = CLOCK_GetIpFreq(obj->clock);

	/*	Default timing configs. Quantum is 10
	 *	config->timingConfig.phaseSeg1 = 3;
	 *	config->timingConfig.phaseSeg2 = 2;
	 *	config->timingConfig.propSeg = 1;
	 *	config->timingConfig.rJumpwidth = 1;
	 */

	/*
	 * There are 10 time quanta per bit.
	 */
	uint32_t clockRate = baud * 10;

	assert(clockRate <= srcClk);

	if (0 == clockRate) {
		clockRate = 1;
	}

	uint32_t priDiv = (srcClk / clockRate) - 1;

	/* Desired baud rate is too low. */
	if (priDiv > 0xFF) {
		priDiv = 0xFF;
	}

	/* clear timing config */
	obj->base->CBT &= ~(CAN_CBT_EPRESDIV_MASK | CAN_CBT_ERJW_MASK
			| CAN_CBT_EPSEG1_MASK | CAN_CBT_EPSEG2_MASK |
			CAN_CBT_EPROPSEG_MASK);

	/* Update Timing Settings. Fixed timing settings*/
	obj->base->CTRL1 |= (CAN_CTRL1_PRESDIV(priDiv)
			| CAN_CTRL1_RJW(1)
			| CAN_CTRL1_PSEG1(3)
			| CAN_CTRL1_PSEG2(2)
			| CAN_CTRL1_PROPSEG(1));

	freeze(false);
}

int16_t can_api::findFirstMBWithCode(uint8_t code, uint8_t mask) {
	for (uint16_t i = 0; i < FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base);
			i++) {
		if(((uint8_t)((obj->base->MB[i].CS & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) & mask) == (uint8_t)code) {
			return i;
		}
	}
	return -1;
}

int16_t can_api::findFirstInactiveMB() {
	for (uint16_t i = 0; i < FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base);
				i++) {
		uint8_t cs = (uint8_t)((obj->base->MB[i].CS & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT);
		if(cs == CS_RX_INACTIVE || cs == CS_TX_INACTIVE) {
			return i;
		}
	}
	return -1;
}

int8_t can_api::readMB(uint16_t mbid, CANMessage& msg) {
	uint32_t cs = obj->base->MB[mbid].CS;
	uint8_t cs_code = (cs & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT;
	if (cs_code == CS_RX_FULL || cs_code == CS_RX_OVERRUN) {
		msg.id = obj->base->MB[mbid].ID & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK);
		msg.format = (cs & CAN_ID_EXT_MASK) ? CanFormat::Extended : CanFormat::Standard;
		msg.type = (cs & CAN_CS_RTR_MASK) ? CanFrameType::Remote : CanFrameType::Data;
		msg.len = (cs & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT;
		msg.dataWord[0] = obj->base->MB[mbid].WORD0;
		msg.dataWord[1] = obj->base->MB[mbid].WORD1;
		return 0;
	}
	return -1;
}

// TODO: make sure this wont break if cs code is busy
uint8_t can_api::clearMB(uint8_t mbid) {
	assert(obj->base->MCR & CAN_MCR_AEN_MASK);
	/* disable interrupts */
	setMBInterrupt(mbid, false);

	/* if there is a transmission pending, abort it */
	if(((obj->base->MB[mbid].CS & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) == CS_TX_DATAREMOTE) {
		obj->base->MB[mbid].CS = CS_TX_ABORT;
		/* wait for abort to complete */
		while(obj->base->IFLAG1 & 1 << mbid)
			;
	}

	/* save abort status */
	uint8_t code = (obj->base->MB[mbid].CS & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT;

	/* clear interrupt flag */
	obj->base->IFLAG1 &= ~(1 << mbid);

	/* clear all fields */
	obj->base->MB[mbid].CS = 0;
	obj->base->MB[mbid].ID = 0;
	obj->base->MB[mbid].WORD0 = 0;
	obj->base->MB[mbid].WORD1 = 0;

	return code;
}

void can_api::setMBInterrupt(uint16_t mbid, bool enabled) {
	if(enabled) {
		obj->base->IMASK1 |= 1 << mbid;
	} else {
		obj->base->IMASK1 &= ~(1 << mbid);
	}
}

void can_api::configTxMB(uint16_t mbid, const CANMessage& msg) {
	/* abort and clear interrupt for mb */
	clearMB(mbid);

	/* write id */
	obj->base->MB[mbid].ID = (msg.format == CanFormat::Extended) ? CAN_ID_EXT(msg.id) : CAN_ID_STD(msg.id);

	/* write payload */
	obj->base->MB[mbid].WORD0 = msg.dataWord[0];
	obj->base->MB[mbid].WORD1 = msg.dataWord[1];

	/* config control and status */
	uint32_t cs_code = CAN_CS_DLC(msg.len);

	if (msg.format == CanFormat::Extended)
	{
		cs_code |= CAN_CS_SRR_MASK | CAN_CS_IDE_MASK;
	}

	if (msg.type == CanFrameType::Remote)
	{
		cs_code |= CAN_CS_RTR_MASK;
	}

	cs_code |= CS_TX_DATAREMOTE;

	/* enable interrupt */
	setMBInterrupt(mbid, true);

	/* activate message buffer */
	obj->base->MB[mbid].CS = cs_code;
}

void can_api::configRxMB(uint16_t mbid, uint32_t id, uint32_t idmask, CanFormat format, CanFrameType type, bool filtertype, bool filterformat) {
	/* abort and clear interrupt for mb */
	clearMB(mbid);

	/* write id */
	obj->base->MB[mbid].ID = (format == CanFormat::Extended) ? CAN_ID_EXT(id) : CAN_ID_STD(id);

	/* config control and status */
	uint32_t cs_code = 0;

	if (format == CanFormat::Extended)
	{
		cs_code |= CAN_CS_SRR_MASK | CAN_CS_IDE_MASK;
	}

	if (type == CanFrameType::Remote)
	{
		cs_code |= CAN_CS_RTR_MASK;
	}

	/* configure filters. Can only be modified in freeze mode*/
	freeze(true);
	/* MG[31] RTR, MG[30] IDE , RXIMR[28:0] ID*/
	obj->base->RXIMR[mbid] =
			(filterformat ? 1 : 0 << 31) |
			(filtertype ? 1 : 0 << 30) |
			(idmask & 0x3FFFFFFF);
	freeze(false);

	cs_code |= CS_RX_EMPTY;

	/* enable interrupt */
	setMBInterrupt(mbid, true);

	/* activate message buffer */
	obj->base->MB[mbid].CS = cs_code;
}
