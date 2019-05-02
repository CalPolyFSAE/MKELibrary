#include "canlight.h"

using namespace BSP;

uint8_t counter;

void cb(void){
	can::CANlight::frame f = can::CANlight::StaticClass().readrx(1);
	if(f.data[0] == 0xff) counter++;
}

void loopbacktest(){

	can::canlight_config c;
	can::CANlight::ConstructStatic(&c);
	can::CANlight& can = can::CANlight::StaticClass();

	can::CANlight::canx_config cx;
	can.init(0, &cx);
	cx.callback = cb;
	can.init(1, &cx);

	can::CANlight::frame f;
	f.data[0] = 0xff;
	f.ext = 1;
	for(uint8_t i = 0; i < 50; i++){
        can.tx(0, f);
	}

    while(1);

}

// Test motor controller response
// Attempts to read CAN offset
can::CANlight::frame out;

void callback(){
    can::CANlight& can = can::CANlight::StaticClass();
    can::CANlight::frame f = can.readrx(0);

    if(f.id == 0x222)
    PRINTF("0x%x 0x%x\n", f.data[4], f.data[5]);
}

void motorcontrollertest(){

    can::canlight_config config;
    can::CANlight::canx_config can0_config;
    can::CANlight::ConstructStatic(&config);
    can::CANlight& can = can::CANlight::StaticClass();
    can0_config.baudRate = 500000;
    can0_config.callback = callback;
    can.init(0, &can0_config);

    out.id = 0x221;
    out.ext = 1;
    out.data[0] = 141;

    can.tx(0, out);
}

can::CANlight::frame out0;
can::CANlight::frame out1;
can::CANlight::frame out2;

void bursttest(){
    SysTick_Config(100000);

    can::canlight_config config;
    can::CANlight::canx_config can0_config;
    can::CANlight::ConstructStatic(&config);
    can::CANlight& can = can::CANlight::StaticClass();
    can0_config.baudRate = 500000;
    can.init(0, &can0_config);

    out0.id = 0x10;
    out1.id = 0x20;
    out2.id = 0x30;

    while(1);

}

extern "C" {
	void SysTick_Handler() {
    can::CANlight& can = can::CANlight::StaticClass();
    can.tx(0, out0);
    can.tx(0, out1);
    can.tx(0, out2);
	}
}
