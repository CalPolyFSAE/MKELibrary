#include "uart.h"
#include "fsl_lpuart.h"

using namespace BSP::uart;

UART::UART(const config* conf){
	clock = conf->clock;
	CLOCK_SetIpSrc(kCLOCK_Lpuart0, clock);
	CLOCK_SetIpSrc(kCLOCK_Lpuart1, clock);
	CLOCK_SetIpSrc(kCLOCK_Lpuart2, clock);
	EnableIRQ(LPUART0_RX_IRQn);
	EnableIRQ(LPUART1_RX_IRQn);
	EnableIRQ(LPUART2_RX_IRQn);
}

void UART::tick(){

}

void UART::init(uint8_t no, UART::uartconfig* conf){

	lpuart_config_t fslconf;
	LPUART_GetDefaultConfig(&fslconf);

	fslconf.baudRate_Bps = conf->baudrate;
	fslconf.enableRx = conf->rxen;
	fslconf.enableTx = conf->txen;
	fslconf.parityMode = (lpuart_parity_mode_t)conf->parity;
	fslconf.stopBitCount = conf->stop == 1 ? kLPUART_OneStopBit : kLPUART_TwoStopBit;
	callbacks[no] = conf->callback;
	flags[no].echo = conf->echo;
    LPUART_Init(base(no), &fslconf, CLOCK_GetIpFreq(clockipnames[no]));
    LPUART_EnableInterrupts(base(no), kLPUART_RxDataRegFullInterruptEnable);
}

void UART::write(uint8_t no, const uint8_t* data, uint16_t len){
	LPUART_WriteBlocking(base(no), data, len);
}

void UART::rxhandler(uint8_t no){
    uint8_t data;

    /* If new data arrived. */
    if ((kLPUART_RxDataRegFullFlag)&LPUART_GetStatusFlags(base(no)))
    {
        data = LPUART_ReadByte(base(no));

        if(flags[no].echo) {
        	LPUART_WriteByte(base(no), data);
        	if(data == '\r') LPUART_WriteByte(base(no), '\n');
        }

        if(callbacks[no]) callbacks[no](data);
    }

    __DSB(); // for good luck

}

extern "C" {
void LPUART0_RX_IRQHandler(void)
{
	UART::StaticClass().rxhandler(0);
}
}
