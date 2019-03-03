#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKE18F16.h"
#include "fsl_debug_console.h"
#include "uart.h"

using namespace BSP;


void tick(void){

}

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
	BOARD_InitDebugConsole();

	uart::config uartc;
	uart::UART::ConstructStatic(&uartc);
	uart::UART& uart = uart::UART::StaticClass();

	uart::UART::uartconfig uart0c;
	uart.init(0, &uart0c);

	uint8_t sout[] = "hi hello abcdefghijklmnopqrstuvwxyz\r\n";
	LPUART_WriteBlocking(LPUART0, sout, sizeof(sout) / sizeof(sout[0]));

    while(1) {
    }
    return 0;
}
