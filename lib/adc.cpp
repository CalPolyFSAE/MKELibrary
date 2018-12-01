#include "adc.h"

using namespace BSP::ADC;

ADC::ADC(const adc_config *config){

	if(config)
	{
		ADC::callbacks[0] = config->callbacks[0];
		ADC::callbacks[1] = config->callbacks[1];
		ADC::port_config = config->port_config;
		ADC::channel_config = config->channel_config;
		ADC::hardware_compare_config = config->hardware_compare_config;
		ADC::hardware_average_mode = config->hardware_average_mode;
		ADC::offset = config->offset;
		ADC::gain = config->gain;
		ADC::dma = config->dma;
		ADC::hardware_trigger = config->hardware_trigger;
	}
}

void ADC::tick(){
	// TODO
}

void ADC::config_port(ADC_Type *base, adc12_config_t *config){
	ADC::port_config = *config;
	ADC12_Init(base, config);
}

void ADC::config_channel(ADC_Type *base, uint32_t channel, adc12_channel_config_t *config){
	ADC::channel_config = *config;
	ADC12_SetChannelConfig(base, channel, config);
}

void ADC::config_hardware_compare(ADC_Type *base, adc12_hardware_compare_config_t *config){
	ADC::hardware_compare_config = *config;
	ADC12_SetHardwareCompareConfig(base, config);
}

void ADC::get_default_config(adc_config *config){
	config->callbacks[0] = NULL;
	config->callbacks[1] = NULL;
	config->port_config = {kADC12_ReferenceVoltageSourceVref, kADC12_ClockSourceAlt0, kADC12_ClockDivider1, kADC12_Resolution12Bit, 17U, true};
	config->channel_config = {0, false};
	config->hardware_compare_config = {kADC12_HardwareCompareMode0, 0, 0};
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

void ADC::set_offset(ADC_Type *base, uint32_t value){
	ADC::offset = value;
	ADC12_SetOffsetValue(base, value);
}

void ADC::set_gain(ADC_Type *base, uint32_t value){
	ADC::gain = value;
	ADC12_SetGainValue(base, value);
}

void ADC::set_hardware_average(ADC_Type *base, adc12_hardware_average_mode_t mode){
	ADC::hardware_average_mode = mode;
	ADC12_SetHardwareAverage(base, mode);
}

void ADC::enable_dma(ADC_Type *base, bool enable){
	ADC::dma = enable;
	ADC12_EnableDMA(base, enable);
}

void ADC::enable_hardware_trigger(ADC_Type *base, bool enable){
	ADC::hardware_trigger = enable;
	ADC12_EnableHardwareTrigger(base, enable);
}

status_t ADC::auto_calibration(ADC_Type *base){
	return(ADC12_DoAutoCalibration(base));
}

uint32_t ADC::read_channel(ADC_Type *base, uint32_t channel){
	return(ADC12_GetChannelConversionValue(base, channel));
}
