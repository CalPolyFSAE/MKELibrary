#include "uart.h"
using namespace BSP;

enum userstate_t {
	offline,
	attached,
	reading
};

static userstate_t userstate;
static uint32_t input;

void cb0(uint8_t data){
	uart::UART& uart = uart::UART::StaticClass();

	switch(userstate){
	case offline:
	{
		if(data == '@'){
			userstate = attached;
			uint8_t message[] = "hello.\r\nenter a number.\r\n";
			uart.write(0, message, sizeof(message)/sizeof(message[0]));
			uart.flags[0].echo = 1;
			input = 0;
		}
	}
	break;
	case attached:
	{
		if(data >= '0' && data <= '9'){
			input *= 10;
			input += (data - '0');
		} else {
			uart.flags[0].echo = 0;
			userstate = reading;
		}
	}
	break;
	case reading:
	{
		uint8_t message[] = "goodbye.\r\n";
		uart.write(0, message, sizeof(message)/sizeof(message[0]));
	}
		break;
	default:
		break;
	}
}

void uarttest(){

	userstate = offline;

	uart::config uartc;
	uart::UART::ConstructStatic(&uartc);
	uart::UART& uart = uart::UART::StaticClass();

	uart::UART::uartconfig uart0c;
	uart0c.callback = cb0;
	uart0c.echo = 0;
	uart.init(0, &uart0c);

}
