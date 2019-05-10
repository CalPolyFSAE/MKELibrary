#include "pin_mux.h"
#include "clock_config.h"
#include "MKE18F16.h"

#include "gpiotests.h"

using namespace BSP;


void tick(void){

}

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();

    tests::togglemany();

    while(1) {
    }
    return 0;
}
