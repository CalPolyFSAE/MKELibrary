/*
 * can.h
 */

#ifndef CAN_H_
#define CAN_H_

#include "Service.h"
#include "fsl_flexcan.h"

namespace BSP::can {

// configuration parameters common to both CAN modules
typedef struct _can_config {
    
    // TODO
    flexcan_clock_source_t clkSrc = kFLEXCAN_ClkSrcPeri;

    // TODO
    clock_name_t clk = kCLOCK_BusClk;

} can_config;

// can driver
class CANlight final : public StaticService<CANlight, const can_config *>{
public:

    // can driver constructor
    // stores config and determines clock frequency
    // config: driver configuration
    CANlight(const can_config * config);

    // configuration parameters for discrete can module
    typedef struct _canx_config {

        // bits per second. default 1Meg.
        uint32_t baudRate = 1000000U;
        // TODO 
        uint8_t enLoopback = 0;
        // TODO
        uint8_t enSelfWake = 0;
        // TODO 
        uint8_t enIndividualMask = 0;
        // callback function 
        void (*callback)(void) = NULL;

    } canx_config;

    // generalized frame struct for tx/rx
    typedef struct _frame {

        // extended id
        uint8_t ext = 0;
        // request frame (no data)
        uint8_t rtr = 0;
        // bytes in frame
        uint8_t dlc = 8;
        // can frame id
        uint32_t id = ~(0U);
        // frame data
        uint8_t data[8] = {0};

    } frame;


    // Initialization function. returns 0 for success
    // bus: CAN0, CAN1
    // config: bus configuration
    uint8_t init(uint8_t bus, canx_config *config);

    // Transmit frame
    // bus: CAN0, CAN1
    // f: completed frame to transmit
    // return: 1 on failure
    uint8_t tx(uint8_t bus, frame f);

    // read received frame. receives data from rxbuffern and formats it
    // bus: CAN0, CAN1
    // return: formatted frame
    frame readrx(uint8_t bus);

    // Callback for interrupt handlers,
    // don't ever call this function.
    // needs to be public for access from IRQHandler
    void mbinterrupt(uint8_t bus);

    // Public flags for unread rx data
    uint8_t unread0 = 0;
    uint8_t unread1 = 0;

private:

    // number of mailboxes. used for iteration
    static constexpr uint8_t mbnum = 16;

    // Convert module number to base address
    static inline CAN_Type* base(uint8_t bus) {
        return bus ? CAN1 : CAN0;
    }

    // storage for can
    can_config driverConfig;

    // clock frequency, assigned in constructor
    uint32_t clockhz = 0;

    // temporary storage for received frames
    flexcan_frame_t rxbuffer0;
    flexcan_frame_t rxbuffer1;

    // storage for callbacks
    void (*callback[2])(void) = {NULL, NULL};

};


}

#endif
