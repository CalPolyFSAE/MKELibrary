#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKE18F16.h"
#include "fsl_debug_console.h"
#include "gpio.h"

#include "tests.h"

//#include "tests.h"


void tick(void){

}

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
	BOARD_InitDebugConsole();

	EnableIRQ(PORTB_IRQn);

    tick();

    BSP::gpio::GPIO::ConstructStatic();

    tests::toggleonce();

    tests::togglemany();

    while(1) {
    }
    return 0;
}
