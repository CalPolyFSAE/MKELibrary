#include "MKE18F16.h"
#include "clock_config.h"
#include "pin_mux.h"

#include "gpio.h"
#include "canlight.h"

#define GEN_CAN_BUS 0
#define GEN_CAN_BAUD_RATE 500000
#define GEN_CAN_LED 15

#define CHG_CAN_BUS 1
#define CHG_CAN_BAUD_RATE 250000
#define CHG_CAN_LED 16

using namespace BSP;

// forwards message to charger CAN bus
static void gen_can_callback() {
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    can::CANlight &can = can::CANlight::StaticClass();
    gpio.toggle(gpio::PortD, GEN_CAN_LED);
    can.tx(CHG_CAN_BUS, can.readrx(GEN_CAN_BUS));
}

// forwards message to general CAN bus
static void chg_can_callback() {
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    can::CANlight &can = can::CANlight::StaticClass();
    gpio.toggle(gpio::PortD, CHG_CAN_LED);
    can.tx(GEN_CAN_BUS, can.readrx(CHG_CAN_BUS));
}

// main program
int main() {
    can::canlight_config config;
    can::CANlight::canx_config canx_config;
    
    // initialize board hardware
    BOARD_InitBootClocks();
    BOARD_InitBootPins();
    
    // initialize GPIO driver
    gpio::GPIO::ConstructStatic();
    gpio::GPIO &gpio = gpio::GPIO::StaticClass();
    
    // configure GPIO outputs
    gpio.out_dir(gpio::PortD, GEN_CAN_LED);
    gpio.out_dir(gpio::PortD, CHG_CAN_LED);
    
    // initialize CAN driver
    can::CANlight::ConstructStatic(&config);
    can::CANlight &can = can::CANlight::StaticClass();
    
    // configure general CAN bus
    canx_config.baudRate = GEN_CAN_BAUD_RATE;
    canx_config.callback = gen_can_callback;
    can.init(GEN_CAN_BUS, &canx_config);
    
    // configure charger CAN bus
    canx_config.baudRate = CHG_CAN_BAUD_RATE;
    canx_config.callback = chg_can_callback;
    can.init(CHG_CAN_BUS, &canx_config);

    while(true);
    return(EXIT_FAILURE);
}
