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

struct CAN::controller_data {
	flexcan_handle_t flexcanHandle;
};

static void flexcan_callback(CAN_Type* base, flexcan_handle_t* handle, status_t status, uint32_t result, void* userData);

CAN::CAN(const config* conf){

}

void CAN::tick() {
	// flag check
}

void CAN::init() {
	// HW init


}

bool CAN::txMSG(const mb_info& frame) {
	return false;
}

bool CAN::rxMSG(const mb_info&, const callback_type&) {
	return false;
}

void CAN::config_controller(CAN_Type* base) {
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

	flexcanConfig.enableIndividMask = true;

	// select clock source and get freq
	flexcan_clock_source_t clk_src;
	clock_name_t clockName;
	if ( /*config value*/0 == clock_source::BUS_CLK) {
		clk_src = kFLEXCAN_ClkSrcPeri;
		clockName = kCLOCK_BusClk;
	} else {
		clk_src = kFLEXCAN_ClkSrcOsc;
		clockName = kCLOCK_ScgSysOscAsyncDiv2Clk;
	}

	uint32_t clock_source_freq = CLOCK_GetFreq(clockName);

	flexcanConfig.clkSrc = clk_src;

	FLEXCAN_Init(CAN0, &flexcanConfig, clock_source_freq);

	/* Create FlexCAN handle structure and set call back function. */
	FLEXCAN_TransferCreateHandle(CAN0, &flexcanHandle, flexcan_callback, NULL);
}

//(*flexcan_transfer_callback_t)(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData);
static void flexcan_callback(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData) {
	// TODO
}

