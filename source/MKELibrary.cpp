#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKE18F16.h"
#include "fsl_debug_console.h"

#include "gpio.h"
#include "adc.h"
#include "candrv.h"
#include "lpit.h"
#include "spi.h"

void tick(void){
}
#include "spi.h"
#include "spitests.h"


using namespace BSP;

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
	BOARD_InitDebugConsole();

    while(1) {
    }

    return 0;
}
