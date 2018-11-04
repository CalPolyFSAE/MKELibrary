#include "spi.h"
#include "fsl_lpspi.h"

using namespace BSP;

SPI::SPI(config* spiconfig) {

    callbacks[0] = spiconfig->callback0;
    callbacks[1] = spiconfig->callback1;

    if(spiconfig->clock0) {
        CLOCK_SetIpSrc(kCLOCK_Lpspi0, spiconfig->clock0);
        freqs[0] = CLOCK_GetIpFreq(kCLOCK_Lpspi0);
    }
    if(spiconfig->clock1) {
        CLOCK_SetIpSrc(kCLOCK_Lpspi1, spiconfig->clock1);
        freqs[1] = CLOCK_GetIpFreq(kCLOCK_Lpspi1);
    }

}

void SPI::initSlave(uint8_t no){

    (void)no;

}

void SPI::initMaster(uint8_t no, masterConfig* mc){


    // Convert custom config struct to the FSL format.
    lpspi_master_config_t fslmc;

    fslmc.baudRate = mc->baudRate;
    fslmc.bitsPerFrame = mc->frameLength;
    fslmc.cpol = mc->cpol;
    fslmc.cpha = mc->cphase;
    fslmc.direction = mc->bitdir;
    fslmc.pcsToSckDelayInNanoSec = 1000000000 * mc->pcsToSck / mc->baudRate;
    fslmc.lastSckToPcsDelayInNanoSec = 1000000000 * mc->lastSckToPcs / mc->baudRate;
    fslmc.betweenTransferDelayInNanoSec = 1000000000 * mc->txToTx / mc->baudRate;
    fslmc.whichPcs = mc->pcs;
    fslmc.pcsActiveHighOrLow = mc->pcspol;
    fslmc.pinCfg = mc->pincfg;
    
    LPSPI_MasterInit(bases[no], &fslmc, freqs[no]);

    handles[no] = new lpspi_master_handle_t;

    LPSPI_MasterTransferCreateHandle(bases[no], (lpspi_master_handle_t*)handles[no], (lpspi_master_transfer_callback_t)callbacks[no], NULL);

}

void SPI::transmit(uint8_t* data, uint8_t size){
    
    lpspi_transfer_t tx;

    tx.txData = data;
    tx.rxData = NULL;
    tx.dataSize = size;
    tx.configFlags = kLPSPI_MasterPcs2;

    LPSPI_MasterTransferNonBlocking(bases[0], (lpspi_master_handle_t*)handles[0], &tx);

}

