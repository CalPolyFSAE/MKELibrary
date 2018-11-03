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
 *
 * What are watermarks?
 *
 * Two important registes: TCR (transmit command) and TDR (transmit data)
 * 
 *
 */

namespace BSP{

class SPI final : public StaticService<SPI, const struct config*> {
public:

    // Base address, master/slave handle pointer, status, user data pointer
    typedef void (*callback)(LPSPI_Type*, void*, status_t, void*);

    struct config {
        // input parameters
        callback callback0;
        callback callback1;
    };

    SPI(const config*);

    void tick() override;
    void init() override;


    void initSlave(uint8_t);
    void initMaster(uint8_t);


private:
    SPI() = default;

    // Handles for each SPI module. Void so as to allow master or slave handle pointers.
    void* handles[2] = {NULL, NULL};
    callback callbacks[2] = {NULL, NULL};

    LPSPI_Type* bases[2] = {LPSPI0, LPSPI1};

};

}

#endif // SPI_H_
