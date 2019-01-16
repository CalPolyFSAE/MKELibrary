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

#define DEMO

#ifdef DEMO
#define DEMO_ADC12_BASE ADC0
#define DEMO_ADC12_CLOCK_SOURCE kADC12_ClockSourceAlt0
#define DEMO_ADC12_IRQn ADC0_IRQn
#define DEMO_ADC12_IRQ_HANDLER_FUNC ADC0_IRQHandler
#define DEMO_ADC12_USER_CHANNEL 12U
#define DEMO_ADC12_CHANNEL_GROUP 0U
#else
#define ADC_BASE ADC0
#define ADC_CH 12
#endif

using namespace BSP;

/*
 * @brief   Application entry point.
 */
int main(void) {
#ifdef DEMO
    adc12_config_t adc12ConfigStruct;
    adc12_channel_config_t adc12ChannelConfigStruct;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Set ADC12's clock source to be Fast IRC async clock. */
    CLOCK_SetIpSrc(kCLOCK_Adc0, kCLOCK_IpSrcFircAsync);

    PRINTF("\r\nADC12 polling Example.\r\n");

    /* Initialize ADC. */
    ADC12_GetDefaultConfig(&adc12ConfigStruct);
    adc12ConfigStruct.clockSource = DEMO_ADC12_CLOCK_SOURCE;
    ADC12_Init(DEMO_ADC12_BASE, &adc12ConfigStruct);

    /* Set to software trigger mode. */
    ADC12_EnableHardwareTrigger(DEMO_ADC12_BASE, false);

    /* Calibrate ADC. */
    if (kStatus_Success != ADC12_DoAutoCalibration(DEMO_ADC12_BASE))
    {
        PRINTF("ADC calibration failed!\r\n");
    }

    /* Trigger the conversion. */
    adc12ChannelConfigStruct.channelNumber = DEMO_ADC12_USER_CHANNEL;
    adc12ChannelConfigStruct.enableInterruptOnConversionCompleted = false;

    while(true)
    {
        /*
         When in software trigger mode, each conversion would be launched once calling the "ADC12_SetChannelConfig()"
         function, which works like writing a conversion command and executing it. For another channel's conversion,
         just to change the "channelNumber" field in channel's configuration structure, and call the
         "ADC12_SetChannelConfig() again.
        */
        ADC12_SetChannelConfig(DEMO_ADC12_BASE, DEMO_ADC12_CHANNEL_GROUP, &adc12ChannelConfigStruct);
        while (0U == (kADC12_ChannelConversionCompletedFlag & ADC12_GetChannelStatusFlags(DEMO_ADC12_BASE, DEMO_ADC12_CHANNEL_GROUP))){}
        PRINTF("ADC Value: %lu\r\n", ADC12_GetChannelConversionValue(DEMO_ADC12_BASE, DEMO_ADC12_CHANNEL_GROUP));
    }
#else
	adc12_channel_config_t config;
	uint32_t data;

	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

  	/* Init FSL debug console. */
	BOARD_InitDebugConsole();

	PRINTF("constructing ADC driver.....");
	ADC::ADC::ConstructStatic(NULL);
	ADC::ADC& adc = ADC::ADC::StaticClass();
	PRINTF("done\n");

	PRINTF("configuring ADC.....\n");
	config.channelNumber = ADC_CH;
	config.enableInterruptOnConversionCompleted = false;
	adc.config_base(ADC_BASE, NULL);
	adc.config_channel(ADC_BASE, 0, &config);
	adc.config_hardware_compare(ADC_BASE, NULL);
	adc.enable_dma(ADC_BASE, false);
	adc.enable_hardware_trigger(ADC_BASE, false);
	PRINTF("done\n");

	PRINTF("calibrating ADC.....\n");
	if(adc.calibrate(ADC_BASE) != kStatus_Success)
		PRINTF("FAILED\n");
	else
		PRINTF("done\n");

    while(true){
    	data = adc.read(ADC_BASE, 0);
    	PRINTF("ADC = %lu\n", data);
    }

    return 0;
#endif
}
