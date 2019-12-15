#include "MKE18F16.h"
#include "pin_mux.h"
#include "clock_config.h"

using namespace BSP;

int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();

    return 0;
}
