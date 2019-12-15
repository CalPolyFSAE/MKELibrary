#include "MKE18F16.h"
#include "clock_config.h"
#include "pin_mux.h"

#include "gpio.h"

using namespace BSP;

int main() {
    // initialize board hardware
    BOARD_InitBootClocks();
    BOARD_InitBootPins();
    
    gpio::GPIO::ConstructStatic();

    while(true);

}

extern "C" {
void SysTick_Handler(void){
	
	gpio::GPIO& gpio = gpio::GPIO::StaticClass();

	gpio.toggle(gpio::PortA, 1);

}
}
