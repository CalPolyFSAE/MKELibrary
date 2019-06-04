#include "forwarder.h"
#include "gpio.h"
#include "canlight.h"

using namespace BSP;

// forwards message from CAN0 to CAN1
static void can0_callback() {
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    can::CANlight &can = can::CANlight::StaticClass();
    gpio.clear(gpio::PortD, CAN0_LED);
    can.tx(1, can.readrx(0));
    gpio.set(gpio::PortD, CAN0_LED);
}

// forwards message from CAN1 to CAN0
static void can1_callback() {
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    can::CANlight &can = can::CANlight::StaticClass();
    gpio.clear(gpio::PortD, CAN1_LED);
    can.tx(0, can.readrx(1));
    gpio.set(gpio::PortD, CAN1_LED);
}

// initializes the CAN forwarder
void forwarder_init() {
    can::canlight_config config;
    can::CANlight::canx_config canx_config;
    
    // initialize GPIO driver
    gpio::GPIO::ConstructStatic();
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    
    // configure GPIO outputs
    gpio.out_dir(gpio::PortD, CAN0_LED);
    gpio.out_dir(gpio::PortD, CAN1_LED);
    
    // initialize CAN driver
    can::CANlight::ConstructStatic(&config);
    can::CANlight &can = can::CANlight::StaticClass();
    
    // configure CAN0
    canx_config.baudRate = CAN0_BAUD_RATE;
    canx_config.callback = can0_callback;
    can.init(0, &canx_config);
    gpio.set(gpio::PortD, CAN0_LED);
    
    // configure CAN1
    canx_config.baudRate = CAN1_BAUD_RATE;
    canx_config.callback = can1_callback;
    can.init(1, &canx_config);
    gpio.set(gpio::PortD, CAN1_LED);
}
