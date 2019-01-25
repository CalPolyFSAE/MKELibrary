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
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "adc.h"

#define ADC_BASE 		ADC0
#define ADC_CHANNEL		12

using namespace BSP;

/*
 * @brief   Application entry point.
 */
int main(void) {
	adc12_channel_config_t channel_config;
	uint32_t data;

	// board initializations
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
	BOARD_InitDebugConsole();

	// create ADC driver
	PRINTF("constructing ADC driver.....");
	ADC::ADC::ConstructStatic(NULL);
	ADC::ADC& adc = ADC::ADC::StaticClass();
	PRINTF("done\n");

	// configure ADC base
	PRINTF("configuring ADC.....");
    adc.config_base(ADC_BASE, NULL);
    adc.set_offset(ADC_BASE, 0);
    adc.set_gain(ADC_BASE, 1);
    adc.set_hardware_average(ADC_BASE, kADC12_HardwareAverageDisabled);
    adc.enable_dma(ADC_BASE, false);
    adc.enable_hardware_trigger(ADC_BASE, false);
    PRINTF("done\n");

    // calibrate ADC
	PRINTF("calibrating ADC.....");
	if(adc.calibrate(ADC_BASE) != kStatus_Success)
	{
		PRINTF("FAILED\n");
		assert(0);
	}
	PRINTF("done\n");

	// configure ADC channel
	PRINTF("configuring ADC channel.....");
	channel_config.channelNumber = ADC_CHANNEL;
	channel_config.enableInterruptOnConversionCompleted = false;
	adc.config_channel(ADC_BASE, 0, &channel_config);
	PRINTF("done\n");

    PRINTF("\npress enter to get ADC value\n");
    while(true)
    {
    	GETCHAR();
    	data = adc.read(ADC_BASE, 0);
    	PRINTF("ADC = %lu\n", data);
    }

    return 0;
}
