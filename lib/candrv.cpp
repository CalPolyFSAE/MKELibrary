/*
 * can.cpp
 *
 *  Created on: Oct 27, 2018
 *      Author: oneso
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

// controller configuration information
struct CAN_drv::Controller_data {
	Controller_data(const Controller_config& config) :
			flexcanHandle(), config(config) {

	}
	flexcan_handle_t flexcanHandle;
	Controller_config config;
};

// fsl_can driver function for callback
static void flexcan_callback(CAN_Type* base, flexcan_handle_t* handle, status_t status, uint32_t result, void* userData);


struct MB_info::Frame_data {
	flexcan_handle_t* handle;
	::flexcan_frame_t fsl_frame = {};
	Frame_data(flexcan_handle_t* handle) : handle(handle) {

	}
};

MB_info::MB_info(Controller bus, uint8_t mb_id) : mb_id(mb_id), bus(bus) {
    // Make sure id is sane
    assert(mb_id < number_MBs);

    // Get relevant configuration from CAN driver
	CAN_drv::Controller_data* conf =
			(bus == Controller::bus_CAN0) ?
					CAN_drv::StaticClass().CAN0_config :
					CAN_drv::StaticClass().CAN1_config;

	flexcan_handle_t* handle = &(conf->flexcanHandle);

    // Associate handle with MB
	frame_data = new Frame_data(handle);
}

MB_info::~MB_info() {
	delete frame_data;
}

void MB_info::reset() {
	CAN_Type* base = bus == Controller::bus_CAN0 ? CAN0 : CAN1;

	flexcan_handle_t* handle = frame_data->handle;

	switch(mode) {

    // Similar behavior for any RX, ditto TX
	case MB_transfer_mode::RX_continuous:
	case MB_transfer_mode::RX_single:
		FLEXCAN_SetRxMbConfig(base, mb_id, nullptr, false);
		FLEXCAN_TransferAbortReceive(base, handle, mb_id);
		break;
	case MB_transfer_mode::TX_continuous:
	case MB_transfer_mode::TX_single:
		FLEXCAN_SetTxMbConfig(base, mb_id, false);
		FLEXCAN_TransferAbortSend(base, handle, mb_id);
		break;
	default:
		break;
	}
	mode = MB_transfer_mode::disabled;
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
			callback((uint8_t*)data, frame_data->fsl_frame.length, bus);
		}
	}
}

/*TODO Check that remote frame is formatted properly
 * it may still need a dlc
 */
bool MB_info::set_frame_data_tx(uint32_t id,
		const uint8_t (&data)[max_msg_size], uint8_t dlc, bool remote,
		bool extended_id) {

	assert(mode == MB_transfer_mode::TX_continuous || mode == MB_transfer_mode::TX_single);

    // TODO < or <= ?
	assert(dlc <= max_msg_size);

    // Determine which CAN module
	CAN_Type* base = (bus == Controller::bus_CAN0) ? CAN0 : CAN1;

    // Assemble frame from arguments
	::flexcan_frame_t frame = {};
	frame.length = remote ? 0 : dlc;
	frame.format = extended_id ? kFLEXCAN_FrameFormatExtend : kFLEXCAN_FrameFormatStandard;
	frame.type = remote ? kFLEXCAN_FrameTypeRemote : kFLEXCAN_FrameTypeData;
	frame.id = extended_id ? FLEXCAN_ID_EXT(id) : FLEXCAN_ID_STD(id);

	// copy over frame data
	uint32_t* dataword0 = (uint32_t*)(&data[0]);
	uint32_t* dataword1 = (uint32_t*)(&data[4]);
	if(!remote) {
        // TODO Shuoldn't start at dataByte0 as written
		frame.dataWord0 = byteSwap<uint32_t>(*dataword0);
		frame.dataWord1 = byteSwap<uint32_t>(*dataword1);
	}

	flexcan_mb_transfer_t transfer = {};
	transfer.frame = &frame;
	transfer.mbIdx = mb_id;

    // TODO This aborts tx; is mb_id sure to be free?
	FLEXCAN_SetTxMbConfig(base, mb_id, true);

	flexcan_handle_t* handle = frame_data->handle;

    // Start transfer or 
	status_t status = FLEXCAN_TransferSendNonBlocking(base, handle, &transfer);

	if (status == kStatus_Success) {
		completed = false;
		return true;
	}
	return false;
}

bool MB_info::set_frame_data_rx(uint32_t id, uint8_t dlc, bool remote,
		bool extended_id, uint32_t mask) {

	assert(mode == MB_transfer_mode::RX_continuous || mode == MB_transfer_mode::RX_single);
    // TODO < or <= ?
	assert(dlc <= max_msg_size);

    // Determine which CAN
	CAN_Type* base = (bus == Controller::bus_CAN0) ? CAN0 : CAN1;

	flexcan_rx_mb_config_t mbConfig = {};
	mbConfig.format = extended_id ? kFLEXCAN_FrameFormatExtend : kFLEXCAN_FrameFormatStandard;
	mbConfig.type = remote ? kFLEXCAN_FrameTypeRemote : kFLEXCAN_FrameTypeData;
	mbConfig.id = extended_id ? FLEXCAN_ID_EXT(id) : FLEXCAN_ID_STD(id);

	FLEXCAN_SetRxMbConfig(base, mb_id, &mbConfig, true);

	FLEXCAN_SetRxIndividualMask(base, mb_id, mask);

	// volatile frame from frame_data used. driver puts data into this struct during interrupt
	flexcan_mb_transfer_t transfer = {};
	transfer.mbIdx = mb_id;
    // TODO why the const_cast?
	transfer.frame = &frame_data->fsl_frame;

	flexcan_handle_t* handle = frame_data->handle;

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
		CAN0_config = new Controller_data(*(conf->CAN0_config));
	if (conf->CAN1_config)
		CAN1_config = new Controller_data(*(conf->CAN1_config));
}

bool CAN_drv::tx_msg(Controller bus, uint32_t id,
		const uint8_t (&data)[max_msg_size], uint8_t dlc, bool remote,
		bool extended_id, const callback_type* callback) {

	MB_info* available = get_free_mb_info(bus);
	if (!available)
		return false;
	if (callback != nullptr && !callback->isNull())
		available->set_callback(*callback);

	available->set_mode(MB_info::MB_transfer_mode::TX_single);
	return available->set_frame_data_tx(id, data, dlc, remote, extended_id);
}

bool CAN_drv::rx_msg(Controller bus, uint32_t id, bool extended_id, uint8_t dlc,
		bool remote, uint32_t mask, const callback_type& callback) {
	assert(!callback.isNull());

	MB_info* available = get_free_mb_info(bus);
	if (!available)
		return false;

	available->set_callback(callback);

	available->set_mode(MB_info::MB_transfer_mode::RX_single);
	return available->set_frame_data_rx(id, dlc, remote, extended_id, mask);
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
    for (i = 0; i < number_MBs; ++i) {
        mb_can0[i] = MB_info(Controller::bus_CAN0, i);
    }
    for (i = 0; i < number_MBs; ++i) {
        mb_can1[i] = MB_info(Controller::bus_CAN1, i);
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

