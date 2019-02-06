#include "spi.h"
#include "fsl_lpspi.h"

using namespace BSP;
using namespace spi;

SPI::SPI(const spi_config* spiconfig) {

    callbacks[0] = spiconfig->callbacks[0];
    callbacks[1] = spiconfig->callbacks[1];

    CLOCK_SetIpSrc(kCLOCK_Lpspi0, spiconfig->clocks[0]);
    freqs[0] = CLOCK_GetIpFreq(kCLOCK_Lpspi0);
    CLOCK_SetIpSrc(kCLOCK_Lpspi1, spiconfig->clocks[1]);
    freqs[1] = CLOCK_GetIpFreq(kCLOCK_Lpspi1);

}

void SPI::tick(){

}
/*
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

    // Necessary to cast handles[] and callbacks[] as those are general purpose 
    // and could be either master or slave constructs
    LPSPI_SlaveTransferCreateHandle(bases[no],
			(lpspi_slave_handle_t*)handles[no],
            (lpspi_slave_transfer_callback_t)callbacks[no], 
            NULL);

}
*/
void SPI::initMaster(uint8_t no, masterConfig* mc){

    // Convert custom config struct to the FSL format.
    lpspi_master_config_t fslmc;

    fslmc.baudRate = mc->baudRate;
    fslmc.bitsPerFrame = mc->frameLength;
    fslmc.cpol = mc->cpol;
    fslmc.cpha = mc->cphase;
    fslmc.direction = mc->bitdir;
    fslmc.pcsToSckDelayInNanoSec = 1000000000U * mc->pcsToSck / mc->baudRate;
    fslmc.lastSckToPcsDelayInNanoSec = 1000000000U * mc->lastSckToPcs / mc->baudRate;
    fslmc.betweenTransferDelayInNanoSec = 1000000000U * mc->txToTx / mc->baudRate;
    fslmc.whichPcs = mc->pcs;
    fslmc.pcsActiveHighOrLow = mc->pcspol;
    fslmc.pinCfg = mc->pincfg;
    fslmc.dataOutConfig = mc->datacfg;

    LPSPI_MasterInit(bases[no], &fslmc, freqs[no]);

    // Configure custom CS pin
    xcvrs[no].csport = mc->csport;
    xcvrs[no].cspin = mc->cspin;
    gpio::GPIO::out_dir(xcvrs[no].csport, xcvrs[no].cspin);
    if(mc->pcspol == kLPSPI_PcsActiveLow) {
        xcvrs[no].csactive = 0;
        gpio::GPIO::set(xcvrs[no].csport, xcvrs[no].cspin);
    } else { 
        xcvrs[no].csactive = 1;
        gpio::GPIO::clear(xcvrs[no].csport, xcvrs[no].cspin);
    }

    // Here part copied from demo lpspi_interrupt_b2b_master
    uint8_t fifosize = LPSPI_GetRxFifoSize(bases[no]);
    uint8_t txwatermark;
    if(fifosize > 1){
        txwatermark = 1;
        xcvrs[no].rxwatermark = fifosize - 2;
    } else {
        txwatermark = 0;
        xcvrs[no].rxwatermark = 0;
    }
        
    LPSPI_SetFifoWatermarks(bases[no], txwatermark, xcvrs[no].rxwatermark);

    LPSPI_Enable(bases[no], false);
    bases[no]->CFGR1 &= (~LPSPI_CFGR1_NOSTALL_MASK);
    LPSPI_Enable(bases[no], true);

    LPSPI_FlushFifo(bases[no], true, true);
    LPSPI_ClearStatusFlags(bases[no], kLPSPI_AllStatusFlag);
    LPSPI_DisableInterrupts(bases[no], kLPSPI_AllInterruptEnable);
    bases[no]->TCR = 
        (bases[no]->TCR &
         ~(LPSPI_TCR_CONT_MASK | LPSPI_TCR_CONTC_MASK | LPSPI_TCR_RXMSK_MASK | LPSPI_TCR_PCS_MASK)) |
        LPSPI_TCR_CONT(0) | LPSPI_TCR_CONTC(0) | LPSPI_TCR_RXMSK(0) | LPSPI_TCR_TXMSK(0) | LPSPI_TCR_PCS(fslmc.whichPcs);

    // Enable interrupt in NVIC
    if(no == 0) EnableIRQ(LPSPI0_IRQn);
    if(no == 1) EnableIRQ(LPSPI1_IRQn);

    // Wait for fifo to clear 
    // TODO move this check somewhere else to avoid while();
    while(LPSPI_GetTxFifoCount(bases[no]) != 0);

}

uint32_t SPI::mastertx(uint8_t no, uint8_t* txdata, uint8_t* rxdata, uint8_t size){

    // copied from lpspi_interrupt_b2b_master
    
    struct transceiver* x = &xcvrs[no];

    

    x->txcount = 0;
    x->txsize = size;
    x->rxcount = 0;

    
    x->rx = rxdata;
    x->tx = txdata;

    uint8_t fifosize = LPSPI_GetRxFifoSize(bases[no]);

    cson(x);

    while((LPSPI_GetTxFifoCount(bases[no]) < fifosize) && 
            (x->txcount - x->rxcount < fifosize)){
        LPSPI_WriteData(bases[no], txdata[x->txcount]);
        x->txcount++;

        if(x->txcount== size) break;
    }

    if(size) x->transmitting = 1;

    LPSPI_EnableInterrupts(bases[no], kLPSPI_RxInterruptEnable);
    return 0;

}
/*
uint32_t SPI::slaverx(uint8_t no, uint8_t* data, uint8_t size){

    lpspi_transfer_t rx;
    rx.txData = NULL;
    rx.rxData = data;
    rx.dataSize = size;
    rx.configFlags = pcs[no] << LPSPI_SLAVE_PCS_SHIFT | kLPSPI_SlaveByteSwap;

    return LPSPI_SlaveTransferNonBlocking(bases[no], (lpspi_slave_handle_t*)handles[no], &rx);

}

uint32_t SPI::slavetx(uint8_t no, uint8_t* data, uint8_t size){
    
    lpspi_transfer_t tx;

    tx.txData = data;
    tx.rxData = NULL;
    tx.dataSize = size;
    tx.configFlags = pcs[no] << LPSPI_SLAVE_PCS_SHIFT | kLPSPI_SlaveByteSwap;

    return LPSPI_SlaveTransferNonBlocking(bases[no], (lpspi_slave_handle_t*)handles[no], &tx);

}
*/
// Copied (mostly) from driver example lpspi_interrupt_b2b_master
void SPI::mastercb(uint8_t no){
    struct transceiver* x = &xcvrs[no];
    if(x->rxcount < x->txsize){
        LPSPI_DisableInterrupts(bases[no], kLPSPI_RxInterruptEnable);
        while((bases[no]->FSR & LPSPI_FSR_RXCOUNT_MASK) >> LPSPI_FSR_RXCOUNT_SHIFT){
            // Read from data register
            x->rx[x->rxcount] = bases[no]->RDR;
            x->rxcount++;
            if(x->rxcount == x->txsize) break;
        }

        if(x->rxcount < x->txsize)
            LPSPI_EnableInterrupts(bases[no], kLPSPI_RxInterruptEnable);
    }

    if((x->txsize - x->rxcount) <= x->rxwatermark){
       bases[no]->FCR = (bases[no]->FCR & (~LPSPI_FCR_RXWATER_MASK)) |
           LPSPI_FCR_RXWATER(((x->txsize - x->rxcount) > 1) ? 
                   ((x->txsize - x->rxcount) - 1U) : (0U));

   }

   if(x->txcount < x->txsize){

       while((LPSPI_GetTxFifoCount(bases[no]) < LPSPI_GetRxFifoSize(bases[no])) && (x->txcount - x->rxcount < (uint8_t)LPSPI_GetRxFifoSize(bases[no]))) {

           LPSPI_WriteData(bases[no], x->tx[x->txcount]);
           x->txcount++;

           if(x->txcount == x->txsize) break;

       }

   }

   if((x->txcount == x->txsize) && (x->rxcount == x->txsize)) {
       x->transmitting = 0;
       LPSPI_DisableInterrupts(bases[no], kLPSPI_AllInterruptEnable);
       csoff(x);
    }

   __DSB();


}

void SPI::slavecb(uint8_t no){
    // todo
}

void SPI::cson(transceiver* x){

    // Toggle once always
    gpio::GPIO::toggle(x->csport, x->cspin);

    // Keep toggling if initial state was wrong
    if(x->csactive == 0 /* active low */)
        while(gpio::GPIO::read(x->csport, x->cspin))
            gpio::GPIO::clear(x->csport, x->cspin);
    else
        while(!gpio::GPIO::read(x->csport, x->cspin))
            gpio::GPIO::set(x->csport, x->cspin);

}

void SPI::csoff(transceiver* x){

    if(x->csactive == 0 /* active low */)
        while(!gpio::GPIO::read(x->csport, x->cspin))
            gpio::GPIO::set(x->csport, x->cspin);
    else
        while(gpio::GPIO::read(x->csport, x->cspin))
            gpio::GPIO::clear(x->csport, x->cspin);

}

extern "C" {

void LPSPI0_IRQHandler(void){
    spi::SPI& spi = spi::SPI::StaticClass();
    if(spi.master[0]) spi.mastercb(0);
    else spi.slavecb(0);
}

void LPSPI1_IRQHandler(void){
    spi::SPI& spi = spi::SPI::StaticClass();
    if(spi.master[0]) spi.mastercb(1);
    else spi.slavecb(1);
}

}
