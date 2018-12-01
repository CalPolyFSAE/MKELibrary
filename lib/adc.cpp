#include "adc.h"

using namespace BSP::ADC;

ADC::ADC(const adc_config *config){
	if(config)
	{
		ADC::config = *config;
	}
	else
	{
		ADC::get_default_config(&(ADC::config));
	}
}

void ADC::tick(){
	// TODO
}

void ADC::config_port(ADC_Type *base){
	ADC12_Init(base, &(ADC::config.port));
}

void ADC::config_channel(ADC_Type *base, uint32_t channel){
	ADC12_SetChannelConfig(base, channel, &(ADC::config.channel));
}

void ADC::config_hardware_compare(ADC_Type *base){
	ADC12_SetHardwareCompareConfig(base, &(ADC::config.hardware_compare));
}

void ADC::set_offset(ADC_Type *base){
	ADC12_SetOffsetValue(base, ADC::config.offset);
}

void ADC::set_gain(ADC_Type *base){
	ADC12_SetGainValue(base, ADC::config.gain);
}

void ADC::set_hardware_average(ADC_Type *base){
	ADC12_SetHardwareAverage(base, ADC::config.hardware_average_mode);
}

void ADC::enable_dma(ADC_Type *base){
	ADC12_EnableDMA(base, ADC::config.dma);
}

void ADC::enable_hardware_trigger(ADC_Type *base){
	ADC12_EnableHardwareTrigger(base, ADC::config.hardware_trigger);
}

void ADC::get_default_config(adc_config *config){
	config->callbacks[0] = NULL;
	config->callbacks[1] = NULL;
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
