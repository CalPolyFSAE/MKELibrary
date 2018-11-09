/*
 * can.cpp
 *
 *  Created on: Oct 27, 2018
 *      Author: oneso
 */

#include "can.h"
#include "fsl_flexcan.h"
#include "fsl_clock.h"

using namespace BSP::CAN;

// controller configuration information
struct CAN::Controller_data {
	Controller_data(const Controller_config& config) :
			flexcanHandle(), config(config) {

	}
	flexcan_handle_t flexcanHandle;
	Controller_config config;
};

// fsl_can driver function for callback
static void flexcan_callback(CAN_Type* base, flexcan_handle_t* handle, status_t status, uint32_t result, void* userData);


CAN::CAN(const can_config* conf) :
		CAN0_config(nullptr), CAN1_config(nullptr), mb_can0(), mb_can1() {
	assert(conf);
	// check if controller is going to be used, then copy out config
	if (conf->CAN0_config)
		CAN0_config = new Controller_data(*(conf->CAN0_config));
	if (conf->CAN1_config)
		CAN1_config = new Controller_data(*(conf->CAN1_config));
}

void CAN::tick() {
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

void CAN::init() {
	// HW init


}

bool CAN::rx_msg(controller bus, const MB_info&, const callback_type&) {
	return false;
}

void CAN::ISR_CAN_driver_RX(controller bus, uint32_t result) {
	assert(result < number_MBs);
	MB_info* mbs = (bus == controller::bus_CAN0) ? mb_can0 : mb_can1;
	mbs[result].completed = true;
}

void CAN::ISR_CAN_driver_TX(controller bus, uint32_t result) {
	assert(result < number_MBs);
	MB_info* mbs = (bus == controller::bus_CAN0) ? mb_can0 : mb_can1;
	mbs[result].completed = true;
}

void CAN::config_controller(Controller_data* config, controller bus) {
	assert(config);

	// select can controller base address
	CAN_Type* base = (bus == controller::bus_CAN0) ? CAN0 : CAN1;

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

//(*flexcan_transfer_callback_t)(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData);
static void flexcan_callback(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData) {
	// result is mb number

	// resolve bus
	CAN::controller bus = (base == CAN0) ? CAN::controller::bus_CAN0 : CAN::controller::bus_CAN1;

	switch (status) {
	case kStatus_FLEXCAN_RxIdle:
		CAN::StaticClass().ISR_CAN_driver_RX(bus, result);
		break;

	case kStatus_FLEXCAN_TxIdle:
		CAN::StaticClass().ISR_CAN_driver_TX(bus, result);
		break;

	default:
		break;
	}
}

