#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdio.h>

#include <Event.h>
#include "Service.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
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
enum  GPIO_port
{
	PortA 	= 0U,	/*!< GPIO Port A*/
	PortB 	= 1U,	/*!< GPIO Port B*/
	PortC 	= 2U,	/*!< GPIO Port C*/
	PortD 	= 3U,	/*!< GPIO Port D*/
	PortE 	= 4U,	/*!< GPIO Port E*/
	PortCount = 5U,	/*!< GPIO Port Count*/
};

/*! @brief GPIO logic level definition */
typedef enum _gpio_logic_level
{
    kGPIO_LogicLow = 0U,  /*!< Digital logic low*/
    kGPIO_LogicHigh = 1U, /*!< Digital logic high*/
} gpio_logic_level_t;

class GPIO final : public StaticService<GPIO> {
public:

	/*! @brief function pointer callback type */
	using ISR_func_ptr = SE::Event<int(uint8_t)>;

	/*! @brief GPIO interrupt function pointers */
    ISR_func_ptr function [GPIO_port::PortCount];

    GPIO();

	void tick() override {
		printf("tock\n");
	}

    /*!
     * @brief Sets the output level of a GPIO pin to logic '1'.
     *
     * @param port 	GPIO port name
     * @param pin	GPIO pin number
     */
    void set(GPIO_port port, uint32_t pin);

    /*!
     * @brief Sets the output level of a GPIO pin to logic '0'.
     *
     * @param port 	GPIO port name
     * @param pin	GPIO pin number
     */
    void clear(GPIO_port port, uint32_t pin);

    /*!
     * @brief Toggles the output logic of a GPIO pin.
     *
     * @param port 	GPIO port name
     * @param pin	GPIO pin number
     */
    void toggle(GPIO_port port, uint32_t pin);

    /*!
     * @brief Reads the current value of a GPIO pin.
     *
     * @param port 	GPIO port name
     * @param pin	GPIO pin number
     * @retval GPIO port input value
     *        - kGPIO_LogicLow: corresponding pin input low-logic level.
     *        - kGPIO_LogicHigh: corresponding pin input high-logic level.
     */
    gpio_logic_level_t read(GPIO_port port, uint32_t pin);

    /*!
     * @brief Sets the port PCR register.
     *
     * This is an example to define an input pin or output pin PCR configuration.
     * @code
     * // Define a digital input pin PCR configuration
     * port_pin_config_t config = {
     *      kPORT_PullUp,
     *      kPORT_FastSlewRate,
     *      kPORT_PassiveFilterDisable,
     *      kPORT_OpenDrainDisable,
     *      kPORT_LowDriveStrength,
     *      kPORT_MuxAsGpio,
     *      kPORT_UnLockRegister,
     * };
     * @endcode
     *
     * @param port 		GPIO port name
     * @param pin		GPIO pin number
     * @param config 	PORT PCR register configuration structure.
     */
    void config_pin(GPIO_port port, uint32_t pin, port_pin_config_t *config);

    /*!
     * @brief Configures the port pin interrupt/DMA request.
     *
     * @param port 		GPIO port name
     * @param pin		GPIO pin number
     * @param config  	PORT pin interrupt configuration.
     *        - #kPORT_InterruptOrDMADisabled: Interrupt/DMA request disabled.
     *        - #kPORT_DMARisingEdge : DMA request on rising edge(if the DMA requests exit).
     *        - #kPORT_DMAFallingEdge: DMA request on falling edge(if the DMA requests exit).
     *        - #kPORT_DMAEitherEdge : DMA request on either edge(if the DMA requests exit).
     *        - #kPORT_FlagRisingEdge : Flag sets on rising edge(if the Flag states exit).
     *        - #kPORT_FlagFallingEdge : Flag sets on falling edge(if the Flag states exit).
     *        - #kPORT_FlagEitherEdge : Flag sets on either edge(if the Flag states exit).
     *        - #kPORT_InterruptLogicZero  : Interrupt when logic zero.
     *        - #kPORT_InterruptRisingEdge : Interrupt on rising edge.
     *        - #kPORT_InterruptFallingEdge: Interrupt on falling edge.
     *        - #kPORT_InterruptEitherEdge : Interrupt on either edge.
     *        - #kPORT_InterruptLogicOne   : Interrupt when logic one.
     *        - #kPORT_ActiveHighTriggerOutputEnable : Enable active high-trigger output (if the trigger states exit).
     *        - #kPORT_ActiveLowTriggerOutputEnable  : Enable active low-trigger output (if the trigger states exit).
     */
    void config_interrupt(GPIO_port port, uint32_t pin, port_interrupt_t config);

    /*!
     * @brief Configures the port interrupt function.
     *
     * @param port 		GPIO port name
     * @param function  User-defined function pointer
     */
    void config_function(GPIO_port port, ISR_func_ptr callback);

private:

    static PORT_Type* get_port(GPIO_port port);
    static GPIO_Type* get_gpio(GPIO_port port);

};


#endif
