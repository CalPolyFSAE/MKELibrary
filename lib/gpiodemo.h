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

volatile uint8_t togglemany_count;

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
	for(uint8_t i = 0; i < 200; i++){
        gpio.toggle(gpio::PortB, 10);
    }

    while(togglemany_count < 200);

    togglemany_count = 0;
	gpio.config_interrupt(gpio::PortB, 11, kPORT_InterruptRisingEdge);
	for(uint8_t i = 0; i < 200; i++){
		gpio.toggle(gpio::PortB, 10);
    }

    while(togglemany_count < 200);

}

void toggleevery_callbacka(){
	printf("port A interrupt triggered\n");
	gpio::GPIO::StaticClass().ack_interrupt(gpio::PortA, 11);
}

void toggleevery_callbackb(){
	printf("port B interrupt triggered\n");
	gpio::GPIO::StaticClass().ack_interrupt(gpio::PortB, 11);
}

void toggleevery_callbackc(){
	printf("port C interrupt triggered\n");
	gpio::GPIO::StaticClass().ack_interrupt(gpio::PortC, 11);
}

void toggleevery_callbackd(){
	printf("port D interrupt triggered\n");
	gpio::GPIO::StaticClass().ack_interrupt(gpio::PortD, 11);
}

void toggleevery_callbacke(){
	printf("port E interrupt triggered\n");
	gpio::GPIO::StaticClass().ack_interrupt(gpio::PortE, 11);
}

void toggleevery(){

	gpio::GPIO& gpio = gpio::GPIO::StaticClass();
	gpio.clear(gpio::PortA, 11);
	gpio.clear(gpio::PortB, 11);
	gpio.clear(gpio::PortC, 11);
	gpio.clear(gpio::PortD, 11);
	gpio.clear(gpio::PortE, 11);
	gpio.config_function(gpio::PortA, toggleevery_callbacka);
	gpio.config_function(gpio::PortB, toggleevery_callbackb);
	gpio.config_function(gpio::PortC, toggleevery_callbackc);
	gpio.config_function(gpio::PortD, toggleevery_callbackd);
	gpio.config_function(gpio::PortE, toggleevery_callbacke);
	gpio.config_interrupt(gpio::PortA, 11, kPORT_InterruptRisingEdge);
	gpio.config_interrupt(gpio::PortB, 11, kPORT_InterruptRisingEdge);
	gpio.config_interrupt(gpio::PortC, 11, kPORT_InterruptRisingEdge);
	gpio.config_interrupt(gpio::PortD, 11, kPORT_InterruptRisingEdge);
	gpio.config_interrupt(gpio::PortE, 11, kPORT_InterruptRisingEdge);
	gpio.set(gpio::PortA, 11);
	gpio.set(gpio::PortB, 11);
	gpio.set(gpio::PortC, 11);
	gpio.set(gpio::PortD, 11);
	gpio.set(gpio::PortE, 11);
}

}

