#include "gpio.h"

using namespace BSP;

namespace tests{

void toggleonce_callback(){
	printf("rising edge received\n");
	gpio::GPIO::StaticClass().ack_interrupt(gpio::PortB, 11);
}

void toggleonce(){

	gpio::GPIO& gpio = gpio::GPIO::StaticClass();
	gpio.clear(gpio::PortB, 10);
	gpio.in_dir(gpio::PortB, 11);
	gpio.config_function(gpio::PortB, toggleonce_callback);
	gpio.config_interrupt(gpio::PortB, 11, kPORT_InterruptRisingEdge);
	gpio.set(gpio::PortB, 10);

}

uint8_t togglemany_count;

void togglemany_callback(){
	togglemany_count++;
	gpio::GPIO::StaticClass().ack_interrupt(gpio::PortB, 11);
}

void togglemany(){
	gpio::GPIO& gpio = gpio::GPIO::StaticClass();
	gpio.clear(gpio::PortB, 10);
	gpio.in_dir(gpio::PortB, 11);
	gpio.config_function(gpio::PortB, togglemany_callback);
	gpio.config_interrupt(gpio::PortB, 11, kPORT_InterruptEitherEdge);
	for(uint8_t i = 0; i < 200; i++)
		gpio.toggle(gpio::PortB, 10);
	printf("%d edges counted\n", togglemany_count);
}

}

