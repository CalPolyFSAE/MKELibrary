#ifndef SPI_H_
#define SPI_H_

#include "Service.h"
#include "gpio.h"
#include "fsl_lpspi.h"

/* 
 * Notes
 *
 * This all relies heavily on the fsl driver using master/slave handles.
 * This makes callbacks a little railroady.
 * Getting away from the dependence on handles is worthwhile but not a priority.
 *
 * Configuration parameters include:
 * - Baud rate
 *   Bits per frame
 *   Clock polarity
 *   Clock phase
 *   MSB/LSB first
 *   Delays: cs to first sck, last sck to !cs, time between transfers
 *      Example sets these to 2000ns
 *   Chip select 0-3, and active high/active low
 *
 * Two important registes: TCR (transmit command) and TDR (transmit data)
 * 
 * 
 */



namespace BSP{

namespace spi{

typedef void (*callback)(void);

struct spi_config {

    // Callback functions
    callback callbacks[2] = {NULL, NULL};
    // Clocks; default behavior invokes kCLOCK_IpSrcFircAsync
    clock_ip_src_t clocks[2] = {kCLOCK_IpSrcFircAsync, kCLOCK_IpSrcFircAsync};

};


class SPI final : public StaticService<SPI, const spi_config*> {
public:

    struct masterConfig {

        uint32_t baudRate = 500000;
        // Number of bits during one Frame, or set of clock pulses
        uint32_t frameLength = 8;

        // SPI Mode; clock polarity/phase, chip select polarity, and bit direction
        lpspi_clock_polarity_t cpol = kLPSPI_ClockPolarityActiveHigh;
        lpspi_clock_phase_t cphase = kLPSPI_ClockPhaseFirstEdge;
        lpspi_pcs_polarity_config_t pcspol = kLPSPI_PcsActiveLow;
        lpspi_shift_direction_t bitdir = kLPSPI_MsbFirst;

        // Number of clocks between events
        uint32_t pcsToSck = 2;
        uint32_t lastSckToPcs = 2;
        uint32_t txToTx = 2;

        // Choice of chip select; 0-3
        lpspi_which_pcs_t pcs = kLPSPI_Pcs0;

        // Not sure what the point of these are.
        lpspi_pin_config_t pincfg = kLPSPI_SdiInSdoOut;
        lpspi_data_out_config_t datacfg = kLpspiDataOutRetained;

        // Defaults for CS
        gpio::GPIO_port csport = gpio::PortA;
        uint8_t cspin = 0;

    };
/*
    struct slaveConfig {

        // Absolutely no idea what this means in a slave context.
        uint32_t frameLength = 8;

        // SPI Mode; clock polarity/phase, chip select polarity, and bit direction
        // Same defaults for master/slave
        lpspi_clock_polarity_t cpol = kLPSPI_ClockPolarityActiveHigh;
        lpspi_clock_phase_t cphase = kLPSPI_ClockPhaseFirstEdge;
        lpspi_pcs_polarity_config_t pcspol = kLPSPI_PcsActiveLow;
        lpspi_shift_direction_t bitdir = kLPSPI_MsbFirst;

        // Choice of chip select; 0-3
        lpspi_which_pcs_t pcs = kLPSPI_Pcs0;

        // Not sure what the point of these are.
        lpspi_pin_config_t pincfg = kLPSPI_SdiInSdoOut;
        lpspi_data_out_config_t datacfg = kLpspiDataOutRetained;
    };
*/
    SPI(const spi_config*);

    // void initSlave(uint8_t no, slaveConfig*);
    void initMaster(uint8_t no, masterConfig*);

    // Arguments: SPI module; data buffer; expected data length
    // Returns state of linked FSL function. 0 is good, not 0 is not good.
    uint32_t mastertx(uint8_t no, uint8_t* txdata, uint8_t* rxdata, uint8_t size);
    
    // Phasing out
    // uint32_t masterrx(uint8_t no, uint8_t* data, uint8_t size);

    // uint32_t slaverx(uint8_t no, uint8_t* data, uint8_t size);
    // uint32_t slavetx(uint8_t no, uint8_t* data, uint8_t size);
    uint8_t master[2] = {1, 1};

    // General callback functions, need to be public to be called from ISR
    void mastercb(uint8_t no);
    void slavecb(uint8_t no);

// private:
    SPI() = default;

    // Flags and info for use during transceiving
    struct transceiver {
        uint8_t txsize = 0;
        uint8_t txcount = 0;
        uint8_t rxcount = 0;
        uint8_t rxwatermark = 0;
        uint8_t transmitting = 0;
        uint8_t* rx = NULL;
        uint8_t* tx = NULL;
        gpio::GPIO_port csport;
        uint8_t cspin;
        uint8_t csactive;
    };


    // Turn CS on or off
    void cson(transceiver*);
    void csoff(transceiver*);

    struct transceiver xcvrs[2];

    // Internal configuration
    callback callbacks[2] = {NULL, NULL};
    uint32_t freqs[2];
    LPSPI_Type* bases[2] = {LPSPI0, LPSPI1};

};

}
}
#endif // SPI_H_
