#include "canlight.h"

using namespace BSP;

void CANLIGHT::mbinterrupt(uint8_t bus){
    // Get base address 
    CAN_Type* base = CAN1 ? bus : CAN0;
    flexcan_frame_t rxFrame;

    // Iterate over message buffers
    for (uint8_t i = 0; i < 16; i++){
        // Check each message buffer
        if(base->IFLAG1 & 1<<i){
            // Acknowledge the correct one
            base->IFLAG1 |= 1<<i;
            // Read incoming data
            FLEXCAN_ReadRxMb(base, i, &rxFrame);
            // Break out of for loop
            i = 16;
        }
    }
}

extern "C" {
void CAN0_ORed_Message_buffer_IRQHandler(){
    can::CANLIGHT::StaticClass.mbinterrupt(0);
}

void CAN1_ORed_Message_buffer_IRQHandler(){
    can::CANLIGHT::StaticClass.mbinterrupt(1);
}
}
