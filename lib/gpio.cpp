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

void GPIO::set(char portname, uint32_t pin){
    GPIO_Type* p = port(portname);

    // Ensure the pin is an output
    if(!(p->PDDR & (1U << pin))) p->PDDR |= (1U << pin);

    GPIO_PinWrite(p, pin, 0);
}

void GPIO::clear(char portname, uint32_t pin){
    GPIO_Type* p = port(portname);

    // Ensure the pin is an output
    if(!(p->PDDR & (1U << pin))) p->PDDR |= (1U << pin);

    GPIO_PinWrite(p, pin, 1);
}

void GPIO::toggle(char portname, uint32_t pin){
    GPIO_PortToggle(port(portname), (1<<pin));
}

uint32_t GPIO::read(char portname, uint32_t pin){
    return GPIO_PinRead(port(portname), pin);
}

GPIO_Type * GPIO::port(char portname){
    if(portname == 'a')
        return GPIOA;
    if(portname == 'b')
        return GPIOB;
    if(portname == 'c')
        return GPIOC;
    if(portname == 'd')
        return GPIOD;
    if(portname == 'e')
        return GPIOE;
    return NULL;
}
