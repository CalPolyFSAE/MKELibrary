/*
 * can.cpp
 */

#include "can.h"

using namespace BSP::can;

CANlight::CANlight(const can_config* conf){

    driverConfig = *conf;
    clockhz = CLOCK_GetFreq(conf->clk);

}

void CANlight::mbinterrupt(uint8_t bus){

    // Get base address 
    CAN_Type* c = base(bus);

    // Iterate over message buffers
    for (uint8_t i = 0; i < 16; i++){
        // Check each message buffer
        if(c->IFLAG1 & 1<<i){
            // Acknowledge the correct one
            c->IFLAG1 |= 1<<i;
            // Read incoming data
            flexcan_frame_t* rxbuffer = bus ? &rxbuffer1 : &rxbuffer0;
            FLEXCAN_ReadRxMb(c, i, rxbuffer);
            // Set unread data flag
            if(bus) unread1 = 1;
            else unread0 = 1;
            // Break out of for loop
            i = 16;
        }
    }
    
    if(callback[bus]) callback[bus]();

    __DSB(); // for good luck
}


uint8_t CANlight::init(uint8_t bus, canx_config* conf){

    callback[bus] = conf->callback;

    // convert configuration to fsl format
    flexcan_config_t fconf;
    FLEXCAN_GetDefaultConfig(&fconf);
    fconf.clkSrc = driverConfig.clkSrc;
    fconf.baudRate = conf->baudRate;
    fconf.maxMbNum = mbnum; 
    fconf.enableLoopBack = conf->enLoopback;
    fconf.enableSelfWakeup = conf->enSelfWake;
    fconf.enableIndividMask = conf->enIndividualMask;
    // run fsl can initialization
    FLEXCAN_Init(base(bus), &fconf, clockhz);

    // set upper 8 MBs to tx mode
    for(uint8_t i = 8; i < 16; i++)
        FLEXCAN_SetTxMbConfig(base(bus), i, true);

    // Configuration not set; setting global mask to 0 should 
    // allow this buffer to receive anything
    // Locking rx mb at 0, top of FIFO
    uint8_t rxmb = 0;
    flexcan_rx_mb_config_t rxmbconf = {0};
    FLEXCAN_SetRxMbConfig(base(bus), rxmb, &rxmbconf, true);
    // Set global rx mask to 0: do not screen any incoming frames
    FLEXCAN_SetRxMbGlobalMask(base(bus), 0);

    // Enable interrupts on specific RX MB
    FLEXCAN_EnableMbInterrupts(base(bus), 1 << rxmb);
    // Enable interrupt in NVIC
    EnableIRQ(bus ? CAN1_ORed_Message_buffer_IRQn : CAN0_ORed_Message_buffer_IRQn);

    return 0;
}


uint8_t CANlight::tx(uint8_t bus, frame f){

    // convert frame to fsl format
    flexcan_frame_t fslf;
    fslf.format = f.ext ? kFLEXCAN_FrameFormatExtend : kFLEXCAN_FrameFormatStandard;
    fslf.type = f.rtr ? kFLEXCAN_FrameTypeRemote : kFLEXCAN_FrameTypeData;
    fslf.id = f.ext ? FLEXCAN_ID_EXT(f.id) : FLEXCAN_ID_STD(f.id);
    fslf.length = f.dlc;
    // convert data to fsl format
    fslf.dataWord0 = CAN_WORD0_DATA_BYTE_0(f.data[0]) | CAN_WORD0_DATA_BYTE_1(f.data[1]) 
        | CAN_WORD0_DATA_BYTE_2(f.data[2]) | CAN_WORD0_DATA_BYTE_3(f.data[3]);
    fslf.dataWord1 = CAN_WORD1_DATA_BYTE_4(f.data[4]) | CAN_WORD1_DATA_BYTE_5(f.data[5]) 
        | CAN_WORD1_DATA_BYTE_6(f.data[6]) | CAN_WORD1_DATA_BYTE_7(f.data[7]);

    // find a free message buffer
    for(uint8_t i = 8; i < mbnum; i++){
        if(CAN_CS_CODE(0xC) != // kFLEXCAN_TxMbDataOrRemote
                (base(bus)->MB[i].CS & CAN_CS_CODE_MASK)){
            // free buffer found
            FLEXCAN_WriteTxMb(base(bus), i, &fslf);
            return 0;
        }
    }

    // failed to find valid buffer
    return 1;

}


CANlight::frame CANlight::readrx(uint8_t bus){

    frame f;
    flexcan_frame_t rxbuffer = bus ? rxbuffer1 : rxbuffer0;
    f.ext = rxbuffer.format;
    f.rtr = rxbuffer.type;
    f.id = rxbuffer.id;
    f.dlc = rxbuffer.length;
    f.data[0] = rxbuffer.dataByte0;
    f.data[1] = rxbuffer.dataByte1;
    f.data[2] = rxbuffer.dataByte2;
    f.data[3] = rxbuffer.dataByte3;
    f.data[4] = rxbuffer.dataByte4;
    f.data[5] = rxbuffer.dataByte5;
    f.data[6] = rxbuffer.dataByte6;
    f.data[7] = rxbuffer.dataByte7;
    if(bus) unread1 = 0;
    else unread0 = 0;
    return f;
}


// Interrupt handlers
extern "C" {
void CAN0_ORed_Message_buffer_IRQHandler(){
    CANlight::StaticClass().mbinterrupt(0);
}

void CAN1_ORed_Message_buffer_IRQHandler(){
    CANlight::StaticClass().mbinterrupt(1);
}
}

