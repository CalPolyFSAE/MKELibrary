#include "MKE18F16.h"
#include "adc.h"

using namespace BSP::ADC;

ADC::ADC(const adc_config_t *config){
	if(config){
		ADC::config[0] = *config;
		ADC::config[1] = *config;
		ADC::config[2] = *config;
	}else{
		ADC::get_default_config(&(ADC::config[0]));
		ADC::get_default_config(&(ADC::config[1]));
		ADC::get_default_config(&(ADC::config[2]));
	}
}

void ADC::tick(){
	// TODO
}

void ADC::config_port(ADC_Type *base, adc12_config_t *config){
	uint32_t index = get_index(base);

	if(config)
		ADC::config[index].port = *config;

	ADC12_Init(base, &(ADC::config[index].port));
}

void ADC::config_channel(ADC_Type *base, uint32_t channel, adc12_channel_config_t *config){
	uint32_t index = get_index(base);

	if(config)
		ADC::config[index].channel = *config;

	ADC12_SetChannelConfig(base, channel, &(ADC::config[index].channel));
}

void ADC::config_hardware_compare(ADC_Type *base, adc12_hardware_compare_config_t *config){
	uint32_t index = get_index(base);

	if(config)
		ADC::config[index].hardware_compare = *config;

	ADC12_SetHardwareCompareConfig(base, &(ADC::config[index].hardware_compare));
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

void ADC::get_default_config(adc_config_t *config){
	config->function = NULL;
	config->port = {kADC12_ReferenceVoltageSourceVref, kADC12_ClockSourceAlt0, kADC12_ClockDivider1, kADC12_Resolution12Bit, 17U, true};
	config->channel= {0, false};
	config->hardware_compare = {kADC12_HardwareCompareMode0, 0, 0};
	config->hardware_average_mode = kADC12_HardwareAverageDisabled;
	config->offset = 0;
	config->gain = 0;
	config->dma = false;
	config->hardware_trigger = false;
}

uint32_t ADC::get_port_status_flags(ADC_Type *base){
	return(ADC12_GetStatusFlags(base));
}

uint32_t ADC::get_channel_status_flags(ADC_Type *base, uint32_t channel){
	return(ADC12_GetChannelStatusFlags(base, channel));
}

status_t ADC::auto_calibration(ADC_Type *base){
	return(ADC12_DoAutoCalibration(base));
}

uint32_t ADC::read_channel(ADC_Type *base, uint32_t channel){
	return(ADC12_GetChannelConversionValue(base, channel));
}

adc_callback_t ADC::get_callback(ADC_Type *base){
	return(ADC::config[get_index(base)].function);
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
void ADC1_IRQHandler(){
	// TODO
}

void ADC2_IRQHandler(){
	// TODO
}
}
