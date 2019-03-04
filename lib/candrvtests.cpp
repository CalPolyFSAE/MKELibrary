/*
 * candrvtests.cpp
 *
 *  Created on: Feb 25, 2019
 *      Author: HB
 */

#include <lib/candrv.h>

using namespace BSP;

void TestCallback(const uint8_t*, uint8_t) {
	printf("Callback\n");
}

void TestCAN() {
	can::Controller_config CAN0_conf = {};
	can::can_config config = {};

	config.CAN0_config = &CAN0_conf;

	can::CAN_drv::ConstructStatic(&config);

	can::CAN_drv& CAN = can::CAN_drv::StaticClass();

	CAN.init();

	//CAN.rx_msg(can::Controller::bus_CAN0, 0xA0, false, 7, false, ~0, can::callback_type::create<TestCallback>());
	uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7};
	if(CAN.tx_msg(can::Controller::bus_CAN0, 0xA0, data, 7, false, false, can::callback_type::create<TestCallback>())) {
		printf("tx true");
	}
}

