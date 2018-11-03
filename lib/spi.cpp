#include "spi.h"
#include "fsl_lpspi.h"

using namespace BSP;

SPI::SPI(const config* config) {

    callbacks[0] = config->callback0;
    callbacks[1] = config->callback1;

}

void SPI::init() {
    
    //pinmux, clock stuff

}


void SPI::initSlave(uint8_t no){

    (void)no;

}

void SPI::initMaster(uint8_t no){

    lpspi_master_config_t masterConfig;

    // Notable defaults:
    // cs = #0
    LPSPI_MasterGetDefaultConfig(&masterConfig);

    LPSPI_MasterInit(bases[no], &masterConfig, CLOCK_GetIpFreq(kCLOCK_Lpspi0));

    LPSPI_MasterTransferCreateHandle(bases[no], (lpspi_master_handle_t*)handles[no], (lpspi_master_transfer_callback_t)callbacks[no], NULL);
    
    LPSPI_Enable(bases[no], false);
    bases[no]->CFGR1 &= (~LPSPI_CFGR1_NOSTALL_MASK);
    LPSPI_Enable(bases[no], true);

}

