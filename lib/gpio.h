#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdio.h>

#include "Service.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "MKE18F16.h"

namespace BSP{
namespace gpio{

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

class GPIO final : public StaticService<GPIO> {
public:

	/*! @brief function pointer callback type */
	using ISR_func_ptr = void (*)(void);

	/*! @brief GPIO interrupt function pointers */
    ISR_func_ptr function [GPIO_port::PortCount];

    GPIO();

	void tick() override {
		printf("tock\n");
	}

    // Make pin an input
    static void in_dir(GPIO_port port, uint32_t pin);

    // Make pin an output
    static void out_dir(GPIO_port port, uint32_t pin);

    /*!
     * @brief Sets the output level of a GPIO pin to logic '1'.
     *
     * @param port 	GPIO port name
     * @param pin	GPIO pin number
     */
    static void set(GPIO_port port, uint32_t pin);

    /*!
     * @brief Sets the output level of a GPIO pin to logic '0'.
     *
     * @param port 	GPIO port name
     * @param pin	GPIO pin number
     */
    static void clear(GPIO_port port, uint32_t pin);

    /*!
     * @brief Toggles the output logic of a GPIO pin.
     *
     * @param port 	GPIO port name
     * @param pin	GPIO pin number
     */
    static void toggle(GPIO_port port, uint32_t pin);

    /*!
     * @brief Reads the current value of a GPIO pin.
     *
     * @param port 	GPIO port name
     * @param pin	GPIO pin number
     * @retval GPIO port input value
     */
    static uint8_t read(GPIO_port port, uint32_t pin);

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

    // Determine first source of an interrupt. Checks each pin in order from 0 to 31.
    // Returns 32 (impossible source) if no source found.
    uint8_t int_source(GPIO_port port);

    // Acknowledge interrupt. This enables another interrupt from this source.
    void ack_interrupt(GPIO_port, uint8_t pin);

private:

    static PORT_Type* get_port(GPIO_port port);
    static GPIO_Type* get_gpio(GPIO_port port);

};
}
}

#endif
