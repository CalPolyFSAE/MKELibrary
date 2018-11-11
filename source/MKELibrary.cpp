/*
 * Copyright 2016-2018 NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    MKELibrary.cpp
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKE18F16.h"
#include "fsl_debug_console.h"
#include "gpio.h"
#include "spi.h"

#define TRANSFER_SIZE 64U         /*! Transfer dataSize */

uint8_t slaveRxData[TRANSFER_SIZE] = {0U};
uint8_t masterRxData[TRANSFER_SIZE] = {0U};
uint8_t masterTxData[TRANSFER_SIZE] = {0U};

volatile bool misTransferCompleted = false;

volatile bool sisTransferCompleted = false;
void LPSPI_SlaveUserCallback(LPSPI_Type *base, void *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        PRINTF("This is LPSPI slave transfer completed callback. \r\n");
        PRINTF("It's a successful transfer. \r\n\r\n");
    }

    if (status == kStatus_LPSPI_Error)
    {
        PRINTF("This is LPSPI slave transfer completed callback. \r\n");
        PRINTF("Error occured in this transfer. \r\n\r\n");
    }

    sisTransferCompleted = true;
}
void LPSPI_MasterUserCallback(LPSPI_Type *base, void *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        __NOP();
    }

    misTransferCompleted = true;
}


using namespace BSP;


int main(void) {
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
	BOARD_InitDebugConsole();

    spi::spi_config spiconf;
    spiconf.callbacks[0] = LPSPI_MasterUserCallback;
    spiconf.callbacks[1] = LPSPI_SlaveUserCallback;
    spiconf.clocks[0] = kCLOCK_IpSrcFircAsync;
    spiconf.clocks[1] = kCLOCK_IpSrcFircAsync;

    spi::SPI::ConstructStatic(&spiconf);
    spi::SPI& spi = spi::SPI::StaticClass();
    uint32_t errorCount;
    uint32_t i;

    spi::SPI::masterConfig mconf;

    mconf.baudRate = 500000U;
    mconf.frameLength = 8 * TRANSFER_SIZE;
    mconf.pcs = kLPSPI_Pcs2;
    spi.initMaster(0, &mconf);

    spi::SPI::slaveConfig sconf;
    sconf.frameLength = 8 * TRANSFER_SIZE;
    sconf.pcs = kLPSPI_Pcs0;
    spi.initSlave(1, &sconf);

	for (i = 0U; i < TRANSFER_SIZE; i++)
	{
	 slaveRxData[i] = 0U;
	}

	sisTransferCompleted = false;
	spi.slaverx(1, slaveRxData, TRANSFER_SIZE);

	for (i = 0U; i < TRANSFER_SIZE; i++)
	{
	  masterTxData[i] = (i) % 256U;
	  masterRxData[i] = 0U;
	}

	/* Print out transmit buffer */
	PRINTF("\r\n Master transmit:\r\n");
	for (i = 0U; i < TRANSFER_SIZE; i++)
	{
	  /* Print 16 numbers in a line */
	  if ((i & 0x0FU) == 0U)
	  {
		  PRINTF("\r\n");
	  }
	  PRINTF(" %02X", masterTxData[i]);
	}
	PRINTF("\r\n");

	misTransferCompleted = false;
	spi.mastertx(0, masterTxData, TRANSFER_SIZE);

	while (!sisTransferCompleted || !misTransferCompleted)
	{
	}

	sisTransferCompleted = false;
	spi.slavetx(1, slaveRxData, TRANSFER_SIZE);
	misTransferCompleted = false;
	spi.masterrx(0, masterRxData, TRANSFER_SIZE);

	while (!sisTransferCompleted || !misTransferCompleted)
	{
	}

	/* Print out receive buffer */
	PRINTF("\r\n Slave received:");
	for (i = 0U; i < TRANSFER_SIZE; i++)
	{
	 /* Print 16 numbers in a line */
	 if ((i & 0x0FU) == 0U)
	 {
		 PRINTF("\r\n    ");
	 }
	 PRINTF(" %02X", slaveRxData[i]);
	}
	PRINTF("\r\n");

	errorCount = 0U;
	for (i = 0U; i < TRANSFER_SIZE; i++)
	{
	 if (masterTxData[i] != masterRxData[i])
	 {
		 errorCount++;
	 }
	}
	if (errorCount == 0U)
	{
	 PRINTF(" \r\nLPSPI transfer all data matched! \r\n");
	 /* Print out receive buffer */
	 PRINTF("\r\n Master received:\r\n");
	 for (i = 0U; i < TRANSFER_SIZE; i++)
	 {
		 /* Print 16 numbers in a line */
		 if ((i & 0x0FU) == 0U)
		 {
			 PRINTF("\r\n");
		 }
		 PRINTF(" %02X", masterRxData[i]);
	 }
	 PRINTF("\r\n");
	}
	else
	{
	 PRINTF(" \r\nError occured in LPSPI transfer ! \r\n");
	}

	PRINTF("done\n");

	while(1){}

    return 0;
}
