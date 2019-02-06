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

using namespace BSP;

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
	BOARD_InitDebugConsole();

    spi::spi_config conf;
    spi::SPI::ConstructStatic(&conf);
    spi::SPI& spi = spi::SPI::StaticClass();

    spi::SPI::masterConfig mconf;
    mconf.csport = gpio::PortE;
    mconf.cspin = 6;
    spi.initMaster(0, &mconf);

    for(uint32_t i = 0; i < 1000U; i++) __NOP();

    uint8_t txdata[12] = {0x00, 0x01, 0x3d, 0x6e, 0xfc, 0x00, 0x00, 0x00, 0x02, 0x00, 0xd4, 0x28};
    uint8_t rxdata[12] = {0};

    spi.mastertx(0, txdata, rxdata, 12);

    while(spi.xcvrs[0].transmitting);
    uint8_t txdata2[12] = {0x00, 0x02, 0x2b, 0x0a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    spi.mastertx(0, txdata2, rxdata, 12);

    while(1) {
    }

    return 0;
}
