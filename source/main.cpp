#include "MKE18F16.h"
#include "clock_config.h"
#include "pin_mux.h"

#include "forwarder.h"

int main() {
    // initialize board hardware
    BOARD_InitBootClocks();
    BOARD_InitBootPins();
    
    // initialize CAN forwarder
    forwarder_init();
    
    while(true);
    return(EXIT_FAILURE);
}
