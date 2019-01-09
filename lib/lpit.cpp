#include "lpit.h"

using namespace BSP::lpit;

LPIT::LPIT(const lpit_config* conf){

    CLOCK_SetIpSrc(kCLOCK_Lpit0, conf->clock);
    CLOCK_EnableClock(kCLOCK_Lpit0);

    if(conf->interrupts){
        EnableIRQ(LPIT0_Ch0_IRQn);
        EnableIRQ(LPIT0_Ch1_IRQn);
        EnableIRQ(LPIT0_Ch2_IRQn);
        EnableIRQ(LPIT0_Ch3_IRQn);
    }

    base->MCR |= LPIT_MCR_M_CEN(1);

}

void LPIT::init(uint8_t ch, uint32_t period, callback cb){

    // Record callback
    callbacks[ch] = cb;

    // Enable interrupt on channel 
    base->MIER |= 1 << ch;

    // Set period
    base->CHANNEL[ch].TVAL = period;

    // Associate the timer's own trigger with it
    base->CHANNEL[ch].TCTRL = LPIT_TCTRL_TRG_SEL(ch) |
    // Restart on reaching 0
        LPIT_TCTRL_TROT(1);
        
    base->CHANNEL[ch].TCTRL |= LPIT_TCTRL_T_EN(1);


}


void LPIT::tick(){

}

void LPIT::interrupt(uint8_t ch){
    if(ch == 0) base->MSR |= LPIT_MSR_TIF0(1);
    if(ch == 1) base->MSR |= LPIT_MSR_TIF1(1);
    if(ch == 2) base->MSR |= LPIT_MSR_TIF2(1);
    if(ch == 3) base->MSR |= LPIT_MSR_TIF3(1);
    
    if(callbacks[ch])
        callbacks[ch]();
}

extern "C" {

void LPIT0_Ch0_IRQHandler(){
    LPIT::StaticClass().interrupt(0);
}

void LPIT0_Ch1_IRQHandler(){
    LPIT::StaticClass().interrupt(1);
}

void LPIT0_Ch2_IRQHandler(){
    LPIT::StaticClass().interrupt(2);
}

void LPIT0_Ch3_IRQHandler(){
    LPIT::StaticClass().interrupt(3);
}

}
