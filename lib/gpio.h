#include <stdio.h>
#include "Service.h"
#include "fsl_gpio.h"

/*
 * General notes
 *
 * Pin config tool must be used to set up PORT utility and associated clocks.
 * This library does not do that, yet.
 *
 * Notes from part reference
 *
 * "Each 32-pin port supports one interrupt."
 * Interrupts defined in startup file for ports A through E
 *      PORTX_IRQHandler
 */

class GPIO : public StaticService<GPIO> {
public:
	GPIO(){
		printf("GPIO INITIALIZED\n");
	}

	virtual void tick() override{
		printf("tock\n");
	}

    // Set a pin to a logic level.
    static void set(char portname, uint32_t pin);
    static void clear(char portname, uint32_t pin);
    // Toggle logic level of pins in mask
    // mask pins 1 and 3: (1 << 0) | (1 << 3)
    static void toggle(char portname, uint32_t pinmask);

private:

    static GPIO_Type * port(char portname);

};
