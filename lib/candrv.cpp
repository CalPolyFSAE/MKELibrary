/*
 * can.cpp
 *
 *  Created on: Oct 27, 2018
 *      Author: HB
 */

#include <candrv.h>
#include "fsl_flexcan.h"
#include "fsl_clock.h"

using namespace BSP::can;

template<>
uint16_t BSP::can::byteSwap<uint16_t>(const uint16_t& val) {
	return (val >> 8) | (val << 8);
}

template<>
uint32_t BSP::can::byteSwap<uint32_t>(const uint32_t& val) {
	return uint32_t(byteSwap<uint16_t>(val) << 16) | byteSwap<uint16_t>(val >> 16);
}

// @brief Hidden internal struct to hold CAN controller information specific to fsl_can
struct CAN_drv::Controller_data {
	Controller_data(CAN_Type* base, const Controller_config& config) :
			base{base}, flexcanHandle{}, config{config} {

	}
	CAN_Type* base;
	flexcan_handle_t flexcanHandle;
	Controller_config config;
};

// fsl_can driver function for callback
static void flexcan_callback(CAN_Type* base, flexcan_handle_t* handle, status_t status, uint32_t result, void* userData);

struct MB_info::Config_data {
	CAN_Type* base = nullptr;
	flexcan_handle_t* flexcanHandle = {};
};

// @brief Hidden internal struct to hold fsl_can specific data structures
struct MB_info::Frame_data {
	::flexcan_frame_t fsl_frame = {};

	flexcan_rx_mb_config_t rxTransfer = {};
	flexcan_mb_transfer_t txTransfer = {};

	Frame_data() = default;
};

MB_info::MB_info(const Config_data* conf, uint8_t mb_id) : mb_id{mb_id} {
    // Make sure id is sane
    assert(mb_id < number_MBs);

    assert(conf);

    // Associate handle with MB
	// TODO: better memory allocation
	frame_data = new Frame_data();
	config_data = new Config_data(*conf);
}

MB_info::~MB_info() {
	delete frame_data;
}

// @brief reset the MB to a known state based on mode
void MB_info::reset(bool disable) {
	CAN_Type* base = config_data->base;

	flexcan_handle_t* handle = config_data->flexcanHandle;

	if(disable) {
		mode = MB_transfer_mode::disabled;
	}

	switch(mode) {

    // Similar behavior for any RX, ditto TX
	case MB_transfer_mode::RX_continuous:
		FLEXCAN_TransferAbortReceive(base, handle, mb_id);
		FLEXCAN_SetRxMbConfig(base, mb_id, &frame_data->rxTransfer, !disable);
		break;
	case MB_transfer_mode::RX_single:
		// disable the message buffer
		mode = MB_transfer_mode::disabled;
		FLEXCAN_TransferAbortReceive(base, handle, mb_id);
		FLEXCAN_SetRxMbConfig(base, mb_id, nullptr, false);
		break;
	case MB_transfer_mode::TX_continuous:
		FLEXCAN_TransferAbortSend(base, handle, mb_id);
		FLEXCAN_SetTxMbConfig(base, mb_id, !disable);
		break;
	case MB_transfer_mode::TX_single:
		// disable the message buffer
		mode = MB_transfer_mode::disabled;
		FLEXCAN_TransferAbortSend(base, handle, mb_id);
		FLEXCAN_SetTxMbConfig(base, mb_id, false);
		break;
	default:
		break;
	}
}

void MB_info::do_callback() {
	if (completed) {
		completed = false;
		if (!callback.isNull()) {
			// TODO I don't think this is gonna work as written.
			// dataByte0 has the fourth-lowest address
			// in the data struct.
			// see drivers/fsl_flexcan.h:353
			uint32_t data[2] = {
					byteSwap<uint32_t>(frame_data->fsl_frame.dataWord0),
					byteSwap<uint32_t>(frame_data->fsl_frame.dataWord1)
			};
			callback((uint8_t*)data, frame_data->fsl_frame.length);
		}
	}
}

/* @brief set mb config for tx data
 */
bool MB_info::set_frame_tx(uint32_t id, bool extended_id, bool tx_continuous) {

	assert(mode == MB_transfer_mode::disabled);

	CAN_Type* base = config_data->base;

    // Assemble frame from arguments
	::flexcan_frame_t& frame = frame_data->fsl_frame;
	frame = {};
	//frame.length = 0; //remote ? 0 : dlc; set this later
	frame.format = extended_id ? kFLEXCAN_FrameFormatExtend : kFLEXCAN_FrameFormatStandard;
	//frame.type = remote ? kFLEXCAN_FrameTypeRemote : kFLEXCAN_FrameTypeData;
	frame.id = extended_id ? FLEXCAN_ID_EXT(id) : FLEXCAN_ID_STD(id);

	flexcan_mb_transfer_t& transfer = frame_data->txTransfer;
	transfer = {};

	transfer.frame = &frame;
	transfer.mbIdx = mb_id;

	if(tx_continuous) {
		mode = MB_transfer_mode::TX_continuous;
	} else {
		mode = MB_transfer_mode::TX_single;
	}

    // TODO This aborts tx; is mb_id sure to be free?
	FLEXCAN_SetTxMbConfig(base, mb_id, true);

	return true;
}

/*
 * @brief start sending data
 * TODO Check that remote frame is formatted properly
 */
bool MB_info::tx_data(const uint8_t (&data)[max_msg_size], uint8_t dlc, bool remote) {
	assert(dlc <= max_msg_size);
	// finish assembling frame from arguments
	::flexcan_frame_t& frame = frame_data->fsl_frame;

	frame.length = remote ? 0 : dlc;
	frame.type = remote ? kFLEXCAN_FrameTypeRemote : kFLEXCAN_FrameTypeData;

	// copy over frame data
	uint32_t* dataword0 = (uint32_t*)(&data[0]);
	uint32_t* dataword1 = (uint32_t*)(&data[4]);
	if(!remote) {
        // TODO Shuoldn't start at dataByte0 as written
		frame.dataWord0 = byteSwap<uint32_t>(*dataword0);
		frame.dataWord1 = byteSwap<uint32_t>(*dataword1);
	}

	// TODO Should this be a critical section?
	// Start transfer
	completed = false;
	status_t status = FLEXCAN_TransferSendNonBlocking(config_data->base, config_data->flexcanHandle, &frame_data->txTransfer);

	if (status == kStatus_Success) {
		return true;
	}
	completed = true;
	return false;
}

bool MB_info::set_frame_rx(uint32_t id, uint8_t dlc, bool remote,
		bool extended_id, uint32_t mask) {

	assert(mode == MB_transfer_mode::disabled);

	assert(dlc <= max_msg_size);

	CAN_Type* base = config_data->base;

	flexcan_rx_mb_config_t& mbConfig = frame_data->rxTransfer;
	mbConfig = {};

	mbConfig.format = extended_id ? kFLEXCAN_FrameFormatExtend : kFLEXCAN_FrameFormatStandard;
	mbConfig.type = remote ? kFLEXCAN_FrameTypeRemote : kFLEXCAN_FrameTypeData;
	mbConfig.id = extended_id ? FLEXCAN_ID_EXT(id) : FLEXCAN_ID_STD(id);

	FLEXCAN_SetRxMbConfig(base, mb_id, &mbConfig, true);

	FLEXCAN_SetRxIndividualMask(base, mb_id, mask);

	// volatile frame from frame_data used. fsl driver puts data into this struct during interrupt
	flexcan_mb_transfer_t& transfer = frame_data->txTransfer;
	transfer = {};

	transfer.mbIdx = mb_id;
	transfer.frame = &frame_data->fsl_frame;

	flexcan_handle_t* handle = config_data->flexcanHandle;

	status_t status = FLEXCAN_TransferReceiveNonBlocking(base, handle,
			&transfer);

	if (status == kStatus_Success)
		return true;
	return false;
}

CAN_drv::CAN_drv(const can_config* conf) :
		CAN0_config(nullptr), CAN1_config(nullptr) {

	assert(conf);
	// check if controller is going to be used, then copy out config
	if (conf->CAN0_config)
		CAN0_config = new Controller_data(CAN0, *(conf->CAN0_config));
	if (conf->CAN1_config)
		CAN1_config = new Controller_data(CAN1, *(conf->CAN1_config));
}

// TODO fix use of set_frame_data_tx
bool CAN_drv::tx_msg(Controller bus, uint32_t id,
		const uint8_t (&data)[max_msg_size], uint8_t dlc, bool remote,
		bool extended_id, callback_type callback) {

	MB_info* available = get_free_mb_info(bus);
	if (!available)
		return false;
	if (!callback.isNull())
		available->set_callback(callback);

	available->set_frame_tx(id, extended_id, false);
	return available->tx_data(data, dlc, remote);
}

// TODO fix use of set_frame_data_rx
bool CAN_drv::rx_msg(Controller bus, uint32_t id, bool extended_id, uint8_t dlc,
		bool remote, uint32_t mask, callback_type callback) {
	assert(!callback.isNull());

	MB_info* available = get_free_mb_info(bus);
	if (!available)
		return false;

	available->set_callback(callback);

	return available->set_frame_rx(id, dlc, remote, extended_id, mask);
}

void CAN_drv::tick() {
	// flag check, mb_info status update, check for hardware/mb_info changes
	if(CAN0_config)
		for (MB_info& mb : mb_can0) {
			mb.do_callback();
		}

    if(CAN1_config)
		for (MB_info& mb : mb_can1) {
			mb.do_callback();
		}
}

void CAN_drv::init() {
	// HW init
	if (CAN0_config)
		config_controller(CAN0_config, Controller::bus_CAN0);
	if (CAN1_config)
		config_controller(CAN1_config, Controller::bus_CAN1);

    uint8_t i = 0;
    MB_info::Config_data c = {};
    for (i = 0; i < number_MBs; ++i) {
    	c.base = CAN0;
    	c.flexcanHandle = &(CAN0_config->flexcanHandle);
        mb_can0[i] = MB_info(&c, i);
    }
    for (i = 0; i < number_MBs; ++i) {
    	c.base = CAN1;
		c.flexcanHandle = &(CAN1_config->flexcanHandle);
        mb_can1[i] = MB_info(&c, i);
    }

}

void CAN_drv::ISR_CAN_driver_RX(Controller bus, uint32_t result) {
	assert(result < number_MBs);
	MB_info* mbs = (bus == Controller::bus_CAN0) ? mb_can0 : mb_can1;
	mbs[result].ISR_set_completed();
}

void CAN_drv::ISR_CAN_driver_TX(Controller bus, uint32_t result) {
	assert(result < number_MBs);
	MB_info* mbs = (bus == Controller::bus_CAN0) ? mb_can0 : mb_can1;
	mbs[result].ISR_set_completed();
}

void CAN_drv::config_controller(Controller_data* config, Controller bus) {
	assert(config);

	// select can controller base address
	CAN_Type* base = (bus == Controller::bus_CAN0) ? CAN0 : CAN1;

	flexcan_config_t flexcanConfig;

	/* Get FlexCAN module default Configuration. */
	/*
	 * flexcanConfig.clkSrc = kFLEXCAN_ClkSrcOsc;
	 * flexcanConfig.baudRate = 1000000U;
	 * flexcanConfig.maxMbNum = 16;
	 * flexcanConfig.enableLoopBack = false;
	 * flexcanConfig.enableSelfWakeup = false;
	 * flexcanConfig.enableIndividMask = false;
	 * flexcanConfig.enableDoze = false;
	 * flexcanConfig.timingConfig = timingConfig;
	 */
	FLEXCAN_GetDefaultConfig(&flexcanConfig);

	// copy config over to driver struct
	flexcanConfig.enableIndividMask = true;
	flexcanConfig.enableLoopBack = config->config.enableLoopback;
	flexcanConfig.baudRate = config->config.baudrate;

	// select clock source and get freq
	flexcan_clock_source_t clk_src;
	clock_name_t clockName;
	if ( config->config.clock == Controller_config::Clock_source::BUS_clk) {
		clk_src = kFLEXCAN_ClkSrcPeri;
		clockName = kCLOCK_BusClk;
	} else {
		clk_src = kFLEXCAN_ClkSrcOsc;
		clockName = kCLOCK_ScgSysOscAsyncDiv2Clk;
	}

	uint32_t clock_source_freq = CLOCK_GetFreq(clockName);
	flexcanConfig.clkSrc = clk_src;

	FLEXCAN_Init(base, &flexcanConfig, clock_source_freq);

	/* Create FlexCAN handle structure and set call back function. */
	FLEXCAN_TransferCreateHandle(base, &(config->flexcanHandle), flexcan_callback, nullptr);
}


// Find first nonbusy MB in CAN0 or CAN1, return its address
MB_info* CAN_drv::get_free_mb_info(Controller bus) {
	MB_info (&mbarr)[number_MBs] = (bus == Controller::bus_CAN0) ? mb_can0 : mb_can1;
	for(uint8_t i = 0; i < number_MBs; ++i) {
		MB_info& mb = mbarr[i];
		if(!mb.is_in_use())
			return &mb;
	}
	return nullptr;
}

//(*flexcan_transfer_callback_t)(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData);
static void flexcan_callback(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData) {
	// result is mb number

	printf("CAN callback\n");

	// resolve bus
	Controller bus = (base == CAN0) ? Controller::bus_CAN0 : Controller::bus_CAN1;

	switch (status) {
	case kStatus_FLEXCAN_RxIdle:
		CAN_drv::StaticClass().ISR_CAN_driver_RX(bus, result);
		break;

	case kStatus_FLEXCAN_TxIdle:
		CAN_drv::StaticClass().ISR_CAN_driver_TX(bus, result);
		break;

	default:
		break;
	}
}

