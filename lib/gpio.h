#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdio.h>
#include "Service.h"
#include "fsl_gpio.h"
#include "MKE18F16.h"

/*
 * General notes
 *
 * The IO system is two peripherals; GPIO and PORT.
 * GPIO handles digital logic reading and writing.
 * PORT handles assigning pins to peripherals and pin interrupts.
 *
 * Pin config tool must be used to set up PORT utility and associated clocks.
 * This library does not do that, yet.
 *
 * Notes from part reference
 *
 * "Each 32-pin port supports one interrupt."
 * Interrupts defined in startup file for ports A through E
 *      PORTX_IRQHandler
 *
 * Pin configuration register
 * One register for each pin: PORTA_PCR0, etc.
 *  ISF: The pin has interrupted
 *      Every ISF is mirrored in port register PORTA_ISFR, etc.
 *  IRQC: When will the pin trigger interrupt?
 *  LK: Lock the register
 *  MUX: What is the pin's function?
 *  DSE: High or low drive strength?
 *  PFE: Passive filter?
 *  PE: Pull enable
 *  PS: Pull direction
 */


class GPIO : public StaticService<GPIO> {
public:
	virtual void tick() override{
		printf("tock\n");
	}

    GPIO();

    // Set a pin to a logic level.
    static void set(char portname, uint32_t pin);
    static void clear(char portname, uint32_t pin);
    // Toggle logic level of pins in mask
    // mask pins 1 and 3: (1 << 0) | (1 << 3)
    static void toggle(char portname, uint32_t pin);

    static uint32_t read(char portname, uint32_t pin);

    // Set to NULL in constructor
    void (*porta_int)(void);

private:

    static GPIO_Type * port(char portname);

};


#endif
