/*
 * can_testing.cpp
 *
 *  Created on: Nov 10, 2018
 *      Author: oneso
 */

#include <candrv.h>
#include <can_testing.h>
#include <stdint.h>

#define CAN_TESTING
#ifdef CAN_TESTING

using namespace BSP::CAN;

void test_RX_event(const uint8_t* data, uint8_t dlc, Controller bus) {

}

class A {
public:
	static void B(int a) {

	}
};

uint8_t testCan() {

	// enable loopback for testing
	Controller_config can0_config;
	can0_config.enableLoopback = true;

	can_config conf;
	conf.CAN0_config = &can0_config;

	CAN_drv::ConstructStatic(&conf);
	CAN_drv &can = CAN_drv::StaticClass();

	// normally called by initializer
	can.init();

	se::Event<void(int)> d;
	auto call = decltype(d)::create<&A::B>();
	//auto callback = callback_type::create<&A::B>();
	//can.rx_msg(Controller::bus_CAN0, 0xDF, false, 7, false, ~(0U), callback);

	return 0;
}

#endif /* CAN_TESTING */
