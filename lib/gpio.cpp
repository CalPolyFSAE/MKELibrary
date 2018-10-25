#include "gpio.h"

extern "C" {
void PORTA_IRQHandler(void) {
	if (GPIO::StaticClass().porta_int)
		GPIO::StaticClass().porta_int();
}
}

GPIO::GPIO(){
    printf("GPIO INITIALIZED\n");
    porta_int = NULL;
}

void GPIO::set(gpio_port_t port, uint32_t pin){
    GPIO_Type* p = get_gpio(port);

    // Ensure the pin is an output
    if(!(p->PDDR & (kGPIO_DigitalOutput << pin))) p->PDDR |= (kGPIO_DigitalOutput << pin);

    GPIO_PortSet(p, (1<<pin));
}

void GPIO::clear(gpio_port_t port, uint32_t pin){
	GPIO_Type* p = get_gpio(port);

    // Ensure the pin is an output
    if(!(p->PDDR & (kGPIO_DigitalOutput << pin))) p->PDDR |= (kGPIO_DigitalOutput << pin);

    GPIO_PortClear(p, (1<<pin));
}

void GPIO::toggle(gpio_port_t port, uint32_t pin){
	GPIO_Type* p = get_gpio(port);

    // Ensure the pin is an output
    if(!(p->PDDR & (kGPIO_DigitalOutput << pin))) p->PDDR |= (kGPIO_DigitalOutput << pin);

    GPIO_PortToggle(p, (1<<pin));
}

uint32_t GPIO::read(gpio_port_t port, uint32_t pin){
    return GPIO_PinRead(get_gpio(port), pin);
}

void GPIO::config_pin(gpio_port_t port, uint32_t pin, port_pin_config_t *config){
	PORT_SetPinConfig(get_port(port), pin, config);
}

void GPIO::config_interrupt(gpio_port_t port, uint32_t pin, port_interrupt_t config){
	PORT_SetPinInterruptConfig(get_port(port), pin, config);
}

PORT_Type * GPIO::get_port(gpio_port_t port){
    switch(port){
    	case kGPIO_PortA:
    		return PORTA;
    	case kGPIO_PortB:
    		return PORTB;
    	case kGPIO_PortC:
    		return PORTC;
    	case kGPIO_PortD:
    		return PORTD;
    	case kGPIO_PortE:
    		return PORTE;
    	default:
    		return NULL;
    }
}

GPIO_Type * GPIO::get_gpio(gpio_port_t port){
    switch(port){
    	case kGPIO_PortA:
    		return GPIOA;
    	case kGPIO_PortB:
    		return GPIOB;
    	case kGPIO_PortC:
    		return GPIOC;
    	case kGPIO_PortD:
    		return GPIOD;
    	case kGPIO_PortE:
    		return GPIOE;
    	default:
    		return NULL;
    }
}
