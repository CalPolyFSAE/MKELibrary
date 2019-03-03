#include "uart.h"
#include "fsl_lpuart.h"

using namespace BSP::uart;

UART::UART(const config* conf){
	clock = conf->clock;
	CLOCK_SetIpSrc(kCLOCK_Lpuart0, clock);
	CLOCK_SetIpSrc(kCLOCK_Lpuart1, clock);
	CLOCK_SetIpSrc(kCLOCK_Lpuart2, clock);

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

    LPUART_Init(base(no), &fslconf, CLOCK_GetIpFreq(clockipnames[no]));

}

