#include "spi.h"
#include "gpio.h"

using namespace BSP;

namespace tests{

volatile uint8_t mastertransferdone;
volatile uint8_t slavetransferdone;

void loop_slavecb(LPSPI_Type *base, void *handle, status_t status, void *userData){
    slavetransferdone = true;
}

void loop_mastercb(LPSPI_Type *base, void *handle, status_t status, void *userData){
    mastertransferdone = true;
}

void loop(){

    uint16_t bytes = 128;
    uint16_t i;

    mastertransferdone = false;
    slavetransferdone = false;

    spi::spi_config spiconf;
    spiconf.callbacks[0] = loop_mastercb;
    spiconf.callbacks[1] = loop_slavecb;
    spiconf.clocks[0] = kCLOCK_IpSrcFircAsync;
    spiconf.clocks[1] = kCLOCK_IpSrcFircAsync;

    spi::SPI::ConstructStatic(&spiconf);
    spi::SPI& spi = spi::SPI::StaticClass();

    spi::SPI::masterConfig mconf;
    mconf.baudRate = 500000U;
    mconf.frameLength = 8 * bytes;
    mconf.pcs = kLPSPI_Pcs2;
    spi.initMaster(0, &mconf);

    spi::SPI::slaveConfig sconf;
    sconf.frameLength = 8 * bytes;
    sconf.pcs = kLPSPI_Pcs0;
    spi.initSlave(1, &sconf);

    uint8_t slaverx[bytes];

    for (i = 0; i < bytes; i++)
        slaverx[i] = 0;

    spi.slaverx(1, slaverx, bytes);

    uint8_t mastertx[bytes];
    uint8_t masterrx[bytes];

    for (i = 0; i < bytes; i++){
        mastertx[i] = i;
        masterrx[i] = 0;
    }

    spi.mastertx(0, mastertx, bytes);

    while(!mastertransferdone || !slavetransferdone);

    mastertransferdone = false;
    slavetransferdone = false;
    spi.slavetx(1, slaverx, bytes);
    spi.masterrx(0, masterrx, bytes);

    while(!mastertransferdone || !slavetransferdone);

    printf("received:\n");
    for(i = 0; i < bytes; i++)
        printf("%02X ", slaverx[i]);
    printf("\n");
    for(i = 0; i < bytes; i++)
        printf("%02X ", mastertx[i]);
    printf("\n");
    for(i = 0; i < bytes; i++)
        printf("%02X ", masterrx[i]);
    printf("\n");

}

// Incomplete
void loop_customcs(){

    uint16_t bytes = 128;
    uint16_t i;

    gpio::GPIO::ConstructStatic();
    gpio::GPIO& gpio = gpio::GPIO::StaticClass();

    gpio.set(gpio::PortE, 6);

    mastertransferdone = false;
    slavetransferdone = false;

    spi::spi_config spiconf;
    spiconf.callbacks[0] = loop_mastercb;
    spiconf.callbacks[1] = loop_slavecb;
    spiconf.clocks[0] = kCLOCK_IpSrcFircAsync;
    spiconf.clocks[1] = kCLOCK_IpSrcFircAsync;

    spi::SPI::ConstructStatic(&spiconf);
    spi::SPI& spi = spi::SPI::StaticClass();

    spi::SPI::masterConfig mconf;
    mconf.baudRate = 500000U;
    mconf.frameLength = 8 * bytes;
    mconf.pcs = kLPSPI_Pcs3;
    spi.initMaster(0, &mconf);

    spi::SPI::slaveConfig sconf;
    sconf.frameLength = 8 * bytes;
    sconf.pcs = kLPSPI_Pcs0;
    spi.initSlave(1, &sconf);

    uint8_t slaverx[bytes];

    for (i = 0; i < bytes; i++)
        slaverx[i] = 0;

    spi.slaverx(1, slaverx, bytes);

    uint8_t mastertx[bytes];
    uint8_t masterrx[bytes];

    for (i = 0; i < bytes; i++){
        mastertx[i] = i;
        masterrx[i] = 0;
    }

    gpio.clear(gpio::PortE, 6);

    spi.mastertx(0, mastertx, bytes);

    while(!mastertransferdone);

    gpio.set(gpio::PortE, 6);

    while(!slavetransferdone);

    gpio.clear(gpio::PortE, 6);

    mastertransferdone = false;
    slavetransferdone = false;
    spi.slavetx(1, slaverx, bytes);
    spi.masterrx(0, masterrx, bytes);

    while(!mastertransferdone);

    gpio.set(gpio::PortE, 6);


    printf("slaverx: ");
    for(i = 0; i < bytes; i++)
        printf("%02X ", slaverx[i]);
    printf("\nmastertx: ");
    for(i = 0; i < bytes; i++)
        printf("%02X ", mastertx[i]);
    printf("\nmasterrx: ");
    for(i = 0; i < bytes; i++)
        printf("%02X ", masterrx[i]);
    printf("\n");

}

void mastersend(){

    uint16_t bytes = 128;
    uint16_t i;

    mastertransferdone = false;

    spi::spi_config spiconf;
    spiconf.callbacks[0] = loop_mastercb;
    spiconf.clocks[0] = kCLOCK_IpSrcFircAsync;

    spi::SPI::ConstructStatic(&spiconf);
    spi::SPI& spi = spi::SPI::StaticClass();

    spi::SPI::masterConfig mconf;
    mconf.baudRate = 500000U;
    mconf.frameLength = 8 * bytes;
    mconf.pcs = kLPSPI_Pcs2;
    spi.initMaster(0, &mconf);

    uint8_t mastertx[bytes];

    for (i = 0; i < bytes; i++){
        mastertx[i] = i;
    }

    spi.mastertx(0, mastertx, bytes);

    while(!mastertransferdone);

    printf("sent.\n");

}


}
