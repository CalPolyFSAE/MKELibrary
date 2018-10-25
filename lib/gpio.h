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

/*! @brief GPIO port definition */
typedef enum _gpio_port
{
	kGPIO_PortA = 0U,	/*!< GPIO Port A*/
	kGPIO_PortB = 1U,	/*!< GPIO Port B*/
	kGPIO_PortC = 2U,	/*!< GPIO Port C*/
	kGPIO_PortD = 3U,	/*!< GPIO Port D*/
	kGPIO_PortE = 4U,	/*!< GPIO Port E*/
} gpio_port_t;

class GPIO : public StaticService<GPIO> {
public:
	virtual void tick() override{
		printf("tock\n");
	}

    GPIO();

    // Set pin
    static void set(gpio_port_t port, uint32_t pin);

    // clear pin
    static void clear(gpio_port_t port, uint32_t pin);

    // Toggle pin
    static void toggle(gpio_port_t port, uint32_t pin);

    // Write pin
    static void write(gpio_port_t port, uint32_t pin, uint32_t data);

    // Read pin
    static uint32_t read(gpio_port_t port, uint32_t pin);

    // Set to NULL in constructor
    void (*porta_int)(void);

private:

    static GPIO_Type * get_port(gpio_port_t port);

};


#endif
