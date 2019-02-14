#ifndef CANLIGHT_H_
#define CANLIGHT_H_

#include "Service.h"
#include "fsl_flexcan.h"

namespace BSP{
namespace can{


// Configuration parameters common to both CAN modules
typedef struct _canlight_config {

   flexcan_clock_source_t clkSrc = kFLEXCAN_ClkSrcPeri;
   clock_name_t clk = kCLOCK_BusClk;

} canlight_config;

class CANlight final : public StaticService<CANlight, const canlight_config *>{
public:

    // Configuration parameters for each discrete module
    typedef struct _canx_config {

        uint32_t baudRate = 1000000U;
        uint8_t enLoopback = 0;
        uint8_t enSelfWake = 0;
        uint8_t enIndividualMask = 0;

    } canx_config;

    // Generalized frame struct for tx/rx
    typedef struct _frame {

        uint8_t ext = 0;
        uint8_t rtr = 0;
        uint8_t dlc = 8;
        uint32_t id = ~(0U);
        uint8_t data[8] = {0};

    } frame;

    CANlight(const canlight_config *);
    void tick() override;

    // Initialization function
    // Returns 0 for success
    uint8_t init(uint8_t bus, canx_config*);

    // Transmit frame
    uint8_t tx(uint8_t bus, frame f);

    // Callback for interrupt handlers,
    // don't ever call this function
    void mbinterrupt(uint8_t bus);

private:

    static constexpr uint8_t mbnum = 16;

    // Convert module number to base address
    static inline CAN_Type* base(uint8_t bus) {
        return bus ? CAN1 : CAN0;
    }

    canlight_config driverConfig;
    uint32_t clockhz = 0;

    flexcan_frame_t rxbuffer;

};


}}


#endif
