# SPI

## Basic Theory

This is a driver for the two low-power SPI modules on the MKE18F. This SPI driver builds on top of the FSL driver, but implements a custom chip select scheme to circumvent the 4-slave maximum built it. Currently, only SPI master mode is supported.

The SPI driver uses two same-length byte arrays for sending and receiving data, which are passed to `mastertx()` to begin transmission. The outgoing array contains the SPI command, plus padding with 0xFF to meet the total communication length. The receiving data fills up with whatever is on the MISO line for each MOSI byte.

Once `mastertx()` is called, the transmission is handled entirely via SPI interrupts. The transceiver flag `transmitting` can be used to monitor the state of the transmission:

```
while(spi.xcvrs[0].transmitting);
```

## Example use

```
// Construct SPI driver with default driver settings
spi::spi_config conf;
spi::SPI::ConstructStatic(&conf);
spi::SPI& spi = spi::SPI::StaticClass();

// Set up master configuration, including CS pin
spi::SPI::masterConfig mconf;
mconf.csport = gpio::PortE;
mconf.cspin = 6;

// Initialize SPI module 0 as master
spi.initMaster(0, &mconf);

// Create tx/rx arrays:
// Anticipated command is 4 MOSI command bytes, followed by 
// 4 MISO data bytes. Total message length is 8
uint8_t tx[8] = {0x00, 0x02, 0x2b, 0x0a, 0xff, 0xff, 0xff, 0xff};
uint8_t rx[8];

// Do SPI command using SPI module 0
spi.mastertx(0, tx, rx, 8);
```
