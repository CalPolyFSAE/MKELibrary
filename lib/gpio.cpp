#include "gpio.h"

GPIO::GPIO() :
		function() {
	printf("GPIO Start\n");
}

void GPIO::set(GPIO_port port, uint32_t pin) {
	GPIO_Type* p = get_gpio(port);

	// Ensure the pin is an output
	if (!(p->PDDR & (kGPIO_DigitalOutput << pin)))
		p->PDDR |= (kGPIO_DigitalOutput << pin);

	GPIO_PortSet(p, (1 << pin));
}

void GPIO::clear(GPIO_port port, uint32_t pin) {
	GPIO_Type* p = get_gpio(port);

	// Ensure the pin is an output
	if (!(p->PDDR & (kGPIO_DigitalOutput << pin)))
		p->PDDR |= (kGPIO_DigitalOutput << pin);

	GPIO_PortClear(p, (1 << pin));
}

void GPIO::toggle(GPIO_port port, uint32_t pin) {
	GPIO_Type* p = get_gpio(port);

	// Ensure the pin is an output
	if (!(p->PDDR & (kGPIO_DigitalOutput << pin)))
		p->PDDR |= (kGPIO_DigitalOutput << pin);

	GPIO_PortToggle(p, (1 << pin));
}

gpio_logic_level_t GPIO::read(GPIO_port port, uint32_t pin){
	if(GPIO_PinRead(get_gpio(port), pin))
		return kGPIO_LogicHigh;
	else
		return kGPIO_LogicLow;
}

void GPIO::config_pin(GPIO_port port, uint32_t pin, port_pin_config_t *config){
	PORT_SetPinConfig(get_port(port), pin, config);
}

void GPIO::config_interrupt(GPIO_port port, uint32_t pin, port_interrupt_t config){
	PORT_SetPinInterruptConfig(get_port(port), pin, config);
}

void GPIO::config_function(GPIO_port port, ISR_func_ptr callback)
{
	if(!callback.isNull())
		function[port] = callback;
	else
		printf("GPIO::config_function got invalid callback");
}

PORT_Type* GPIO::get_port(GPIO_port port) {
	switch (port) {
	case GPIO_port::PortA:
		return PORTA;
	case GPIO_port::PortB:
		return PORTB;
	case GPIO_port::PortC:
		return PORTC;
	case GPIO_port::PortD:
		return PORTD;
	case GPIO_port::PortE:
		return PORTE;
	default:
		return 0;
	}
}

GPIO_Type* GPIO::get_gpio(GPIO_port port) {
	switch (port) {
	case GPIO_port::PortA:
		return GPIOA;
	case GPIO_port::PortB:
		return GPIOB;
	case GPIO_port::PortC:
		return GPIOC;
	case GPIO_port::PortD:
		return GPIOD;
	case GPIO_port::PortE:
		return GPIOE;
	default:
		return 0;
	}
}

extern "C" {

void PORTA_IRQHandler(void) {
	if (!GPIO::StaticClass().function[GPIO_port::PortA].isNull())
		GPIO::StaticClass().function[GPIO_port::PortA](0);
}

void PORTB_IRQHandler(void) {
	if (!GPIO::StaticClass().function[GPIO_port::PortB].isNull())
		GPIO::StaticClass().function[GPIO_port::PortB](0);
}

void PORTC_IRQHandler(void) {
	if (!GPIO::StaticClass().function[GPIO_port::PortC].isNull())
		GPIO::StaticClass().function[GPIO_port::PortC](0);
}

void PORTD_IRQHandler(void) {
	if (!GPIO::StaticClass().function[GPIO_port::PortD].isNull())
		GPIO::StaticClass().function[GPIO_port::PortD](0);
}

void PORTE_IRQHandler(void) {
	if (!GPIO::StaticClass().function[GPIO_port::PortE].isNull())
		GPIO::StaticClass().function[GPIO_port::PortE](0);
}
}

