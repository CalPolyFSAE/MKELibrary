/*
 * adc.c
 */

#include "MKE18F16.h"
#include "adc.h"

using namespace BSP::adc;

ADC::ADC(const adc_config_t *config){

    ADC::config[0] = *config;
    ADC::config[1] = *config;
    ADC::config[2] = *config;

	ADC::config_base(ADC0, &(ADC::config[0].base_config));
	if(ADC::calibrate(ADC0) != kStatus_Success) for(;;);

	ADC::config_base(ADC1, &(ADC::config[1].base_config));
	if(ADC::calibrate(ADC1) != kStatus_Success) for(;;);

	ADC::config_base(ADC2, &(ADC::config[2].base_config));
	if(ADC::calibrate(ADC2) != kStatus_Success) for(;;);
}


void ADC::config_base(ADC_Type *base, adc12_config_t *config){
	uint32_t index = get_index(base);

	if(config)
		ADC::config[index].base_config = *config;

	if(base == ADC0){
		CLOCK_SetIpSrc(kCLOCK_Adc0, ADC::config[index].clock_source);
	}else if(base == ADC1){
		CLOCK_SetIpSrc(kCLOCK_Adc1, ADC::config[index].clock_source);
	}else if(base == ADC2){
		CLOCK_SetIpSrc(kCLOCK_Adc2, ADC::config[index].clock_source);
	}else{
		for(;;);
	}

	ADC12_Init(base, &(ADC::config[index].base_config));
}


void ADC::config_hardware_compare(ADC_Type *base, adc12_hardware_compare_config_t *config){
	uint32_t index = get_index(base);

	if(config)
		ADC::config[index].hardware_compare_config = *config;

	ADC12_SetHardwareCompareConfig(base, &(ADC::config[index].hardware_compare_config));
}


void ADC::set_callback(ADC_Type *base, adc_callback_t function){
	uint32_t index = get_index(base);
	ADC::config[index].function = function;
}


void ADC::set_offset(ADC_Type *base, uint32_t value){
	uint32_t index = get_index(base);
	ADC::config[index].offset = value;
	ADC12_SetOffsetValue(base, ADC::config[index].offset);
}


void ADC::set_gain(ADC_Type *base, uint32_t value){
	uint32_t index = get_index(base);
	ADC::config[index].gain = value;
	ADC12_SetGainValue(base, ADC::config[index].gain);
}


void ADC::set_hardware_average(ADC_Type *base, adc12_hardware_average_mode_t mode){
	uint32_t index = get_index(base);
	ADC::config[index].hardware_average_mode = mode;
	ADC12_SetHardwareAverage(base, ADC::config[index].hardware_average_mode);
}


void ADC::enable_dma(ADC_Type *base, bool enable){
	uint32_t index = get_index(base);
	ADC::config[index].dma = enable;
	ADC12_EnableDMA(base, ADC::config[index].dma);
}


void ADC::enable_hardware_trigger(ADC_Type *base, bool enable){
	uint32_t index = get_index(base);
	ADC::config[index].hardware_trigger = enable;
	ADC12_EnableHardwareTrigger(base, ADC::config[index].hardware_trigger);
}


adc_callback_t ADC::get_callback(ADC_Type *base){
	return(ADC::config[get_index(base)].function);
}


uint32_t ADC::get_base_status_flags(ADC_Type *base){
	return(ADC12_GetStatusFlags(base));
}

uint32_t ADC::get_channel_status_flags(ADC_Type *base, uint32_t group){
	return(ADC12_GetChannelStatusFlags(base, group));
}

status_t ADC::calibrate(ADC_Type *base){
	return(ADC12_DoAutoCalibration(base));
}

uint32_t ADC::read(ADC_Type *base, uint32_t ch){
	uint32_t index = get_index(base);

	ADC::config[index].channel_config = {ch, false};

	if(!(ADC::config[index].hardware_trigger))
		ADC12_SetChannelConfig(base, 0, &(ADC::config[index].channel_config));

    while(!(ADC12_GetChannelStatusFlags(base, 0) & kADC12_ChannelConversionCompletedFlag)){}

	return(ADC12_GetChannelConversionValue(base, 0));
}

uint32_t ADC::get_index(ADC_Type *base){
	if(base == ADC0){
		return 0;
	}else if(base == ADC1){
		return 1;
	}else if(base == ADC2){
		return 2;
	}else{
		assert(0);
	}
}

extern "C" {

// interrupt handler for ADC0
void ADC0_IRQHandler(){
	ADC::StaticClass().get_callback(ADC0)(ADC0, NULL);
}

// interrupt handler for ADC1
void ADC1_IRQHandler(){
	ADC::StaticClass().get_callback(ADC1)(ADC1, NULL);
}

// interrupt handler for ADC2
void ADC2_IRQHandler(){
	ADC::StaticClass().get_callback(ADC2)(ADC2, NULL);
}

}
