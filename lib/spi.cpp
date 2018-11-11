#include "spi.h"
#include "fsl_lpspi.h"

using namespace BSP;
using namespace spi;

SPI::SPI(const spi_config* spiconfig) {

    callbacks[0] = spiconfig->callbacks[0];
    callbacks[1] = spiconfig->callbacks[1];

    if(spiconfig->clocks[0]) {
        // Shove the clock value somewhere in the Clock Table.
        // Fine if done at any point before starting to use the peripheral.
        CLOCK_SetIpSrc(kCLOCK_Lpspi0, spiconfig->clocks[0]);
        freqs[0] = CLOCK_GetIpFreq(kCLOCK_Lpspi0);
    }
    if(spiconfig->clocks[1]) {
        CLOCK_SetIpSrc(kCLOCK_Lpspi1, spiconfig->clocks[1]);
        freqs[1] = CLOCK_GetIpFreq(kCLOCK_Lpspi1);
    }

}

void SPI::tick(){

}

void SPI::initSlave(uint8_t no, slaveConfig* sc){

    // Convert struct to the FSL format
    
    lpspi_slave_config_t fslsc;

    fslsc.bitsPerFrame = sc->frameLength;
    fslsc.cpol = sc->cpol;
    fslsc.cpha = sc->cphase;
    fslsc.whichPcs = sc->pcs;
    pcs[no] = sc->pcs;
    fslsc.direction = sc->bitdir;
    fslsc.pcsActiveHighOrLow = sc->pcspol;
    fslsc.pinCfg = sc->pincfg;
    fslsc.dataOutConfig = sc->datacfg;

    LPSPI_SlaveInit(bases[no], &fslsc);

    handles[no] = new lpspi_slave_handle_t;

    // Necessary to cast hanldes[] and callbacks[] as those are general purpose 
    // and could be either master or slave constructs
    LPSPI_SlaveTransferCreateHandle(bases[no],
			(lpspi_slave_handle_t*)handles[no],
            (lpspi_slave_transfer_callback_t)callbacks[no], 
            NULL);

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
    pcs[no] = mc->pcs;
    fslmc.pcsActiveHighOrLow = mc->pcspol;
    fslmc.pinCfg = mc->pincfg;
    fslmc.dataOutConfig = mc->datacfg;
    
    LPSPI_MasterInit(bases[no], &fslmc, freqs[no]);

    handles[no] = new lpspi_master_handle_t;

    LPSPI_MasterTransferCreateHandle(bases[no], 
    		(lpspi_master_handle_t*)handles[no],
            (lpspi_master_transfer_callback_t)callbacks[no], NULL);

}

void SPI::mastertx(uint8_t no, uint8_t* data, uint8_t size){
    
    lpspi_transfer_t tx;

    tx.txData = data;
    tx.rxData = NULL;
    tx.dataSize = size;
    tx.configFlags = pcs[no] << LPSPI_MASTER_PCS_SHIFT | kLPSPI_MasterPcsContinuous | kLPSPI_MasterByteSwap;

    LPSPI_MasterTransferNonBlocking(bases[no], (lpspi_master_handle_t*)handles[no], &tx);

}

void SPI::masterrx(uint8_t no, uint8_t* data, uint8_t size){
    
    lpspi_transfer_t rx;

    rx.txData = NULL;
    rx.rxData = data;
    rx.dataSize = size;
    rx.configFlags = pcs[no] << LPSPI_MASTER_PCS_SHIFT | kLPSPI_MasterPcsContinuous | kLPSPI_MasterByteSwap;

    LPSPI_MasterTransferNonBlocking(bases[no], (lpspi_master_handle_t*)handles[no], &rx);

}

void SPI::slaverx(uint8_t no, uint8_t* data, uint8_t size){

    lpspi_transfer_t rx;
    rx.txData = NULL;
    rx.rxData = data;
    rx.dataSize = size;
    rx.configFlags = pcs[no] << LPSPI_SLAVE_PCS_SHIFT | kLPSPI_SlaveByteSwap;

    LPSPI_SlaveTransferNonBlocking(bases[no], (lpspi_slave_handle_t*)handles[no], &rx);

}

void SPI::slavetx(uint8_t no, uint8_t* data, uint8_t size){
    
    lpspi_transfer_t tx;

    tx.txData = data;
    tx.rxData = NULL;
    tx.dataSize = size;
    tx.configFlags = pcs[no] << LPSPI_SLAVE_PCS_SHIFT | kLPSPI_SlaveByteSwap;

    LPSPI_SlaveTransferNonBlocking(bases[no], (lpspi_slave_handle_t*)handles[no], &tx);

}

