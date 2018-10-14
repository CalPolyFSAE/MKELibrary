#include "gpio.h"

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

void GPIO::toggle(char portname, uint32_t pinmask){
    GPIO_PortToggle(port(portname), pinmask);
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
