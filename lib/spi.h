#ifndef SPI_H_
#define SPI_H_

#include "Service.h"
#include "fsl_lpspi.h"

/* 
 * Notes
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

class SPI { //final : public StaticService<SPI> {
public:

    // Base address, master/slave handle pointer, status, user data pointer
    typedef void (*callback)(LPSPI_Type*, void*, status_t, void*);

    struct config {
        // Callback functions
        callback callback0 = NULL;
        callback callback1 = NULL;

        // A good choice for this is kCLOCK_IpSrcFircAsync
        clock_ip_src_t clock0 = kCLOCK_IpSrcNoneOrExt;
        clock_ip_src_t clock1 = kCLOCK_IpSrcNoneOrExt;

    };

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

    };

    SPI(config*);


    void initSlave(uint8_t);
    void initMaster(uint8_t, masterConfig*);

    void transmit(uint8_t*, uint8_t);


private:
    SPI() = default;

    // Handles for each SPI module. Void so as to allow master or slave handle pointers.
    void* handles[2] = {NULL, NULL};
    callback callbacks[2] = {NULL, NULL};
    uint32_t freqs[2];
    LPSPI_Type* bases[2] = {LPSPI0, LPSPI1};

};

}

#endif // SPI_H_
