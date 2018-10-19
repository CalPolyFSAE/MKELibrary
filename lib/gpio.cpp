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
    if(!(p->PDDR & (kGPIO_DigitalOutput << pin))) p->PDDR |= (kGPIO_DigitalOutput << pin);

    GPIO_PinWrite(p, pin, kGPIO_LogicHigh);
}

void GPIO::clear(char portname, uint32_t pin){
    GPIO_Type* p = port(portname);

    // Ensure the pin is an output
    if(!(p->PDDR & (kGPIO_DigitalOutput << pin))) p->PDDR |= (kGPIO_DigitalOutput << pin);

    GPIO_PinWrite(p, pin, kGPIO_LogicLow);
}

void GPIO::toggle(char portname, uint32_t pin){
    GPIO_Type* p = port(portname);

    // Ensure the pin is an output
    if(!(p->PDDR & (kGPIO_DigitalOutput << pin))) p->PDDR |= (kGPIO_DigitalOutput << pin);

    GPIO_PortToggle(p, (1<<pin));
}

uint32_t GPIO::read(char portname, uint32_t pin){
    return GPIO_PinRead(port(portname), pin);
}

GPIO_Type * GPIO::port(char portname){
    if(portname == 'a' || portname == 'A')
        return GPIOA;
    if(portname == 'b' || portname == 'B')
        return GPIOB;
    if(portname == 'c' || portname == 'C')
        return GPIOC;
    if(portname == 'd' || portname == 'D')
        return GPIOD;
    if(portname == 'e' || portname == 'E')
        return GPIOE;
    return NULL;
}
