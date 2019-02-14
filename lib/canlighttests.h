#include "canlight.h"

using namespace BSP::can;

uint8_t counter;

void cb(void){
	CANlight::frame f = CANlight::StaticClass().readrx(1);
	if(f.data[0] == 0xff) counter++;
}

void loopbacktest(){

	canlight_config c;
	CANlight::ConstructStatic(&c);
	CANlight& can = CANlight::StaticClass();

	CANlight::canx_config cx;
	can.init(0, &cx);
	cx.callback = cb;
	can.init(1, &cx);

	CANlight::frame f;
	f.data[0] = 0xff;
	f.ext = 1;
	for(uint8_t i = 0; i < 50; i++){
        can.tx(0, f);
	}

    while(1);

}
