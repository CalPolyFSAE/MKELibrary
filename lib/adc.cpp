#include "MKE18F16.h"
#include "adc.h"

using namespace BSP::adc;

// constructs ADC driver
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

	ADC::config_base(ADC0, &(ADC::config[0].base_config));
	if(ADC::calibrate(ADC0) != kStatus_Success) assert(0);

	ADC::config_base(ADC1, &(ADC::config[1].base_config));
	if(ADC::calibrate(ADC1) != kStatus_Success) assert(0);

	ADC::config_base(ADC2, &(ADC::config[2].base_config));
	if(ADC::calibrate(ADC2) != kStatus_Success) assert(0);
}

// routine ADC procedures
void ADC::tick(){}

// configures an ADC
// g: I think it's kind of inconsistent that if you don't use NULL as the
// g: adc_config_t argument when calling ConstructStatic, you can't use
// g: NULL here to get a default base_config. (specifically, if(config) will
// g: fail, so NULL will get passed to ADC12_Init, which will assert(NULL) and fail) 
//
// g: also small thing here but using fsl type adc12_config_t means some 
// g: configuration types will be in namespace ADC and some won't. can we 
// g: forward-declare this into ADC namespace or something?
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
		assert(0);
	}

	ADC12_Init(base, &(ADC::config[index].base_config));
}

// configures the hardware compare for an ADC
void ADC::config_hardware_compare(ADC_Type *base, adc12_hardware_compare_config_t *config){
	uint32_t index = get_index(base);

	if(config)
		ADC::config[index].hardware_compare_config = *config;

	ADC12_SetHardwareCompareConfig(base, &(ADC::config[index].hardware_compare_config));
}

// sets the callback function for an ADC
void ADC::set_callback(ADC_Type *base, adc_callback_t function){
	uint32_t index = get_index(base);
	ADC::config[index].function = function;
}

// sets the offset for an ADC
void ADC::set_offset(ADC_Type *base, uint32_t value){
	uint32_t index = get_index(base);
	ADC::config[index].offset = value;
	ADC12_SetOffsetValue(base, ADC::config[index].offset);
}

// sets the gain for an ADC
void ADC::set_gain(ADC_Type *base, uint32_t value){
	uint32_t index = get_index(base);
	ADC::config[index].gain = value;
	ADC12_SetGainValue(base, ADC::config[index].gain);
}

// sets the hardware averaging mode for an ADC
void ADC::set_hardware_average(ADC_Type *base, adc12_hardware_average_mode_t mode){
	uint32_t index = get_index(base);
	ADC::config[index].hardware_average_mode = mode;
	ADC12_SetHardwareAverage(base, ADC::config[index].hardware_average_mode);
}

// enables DMA for an ADC
void ADC::enable_dma(ADC_Type *base, bool enable){
	uint32_t index = get_index(base);
	ADC::config[index].dma = enable;
	ADC12_EnableDMA(base, ADC::config[index].dma);
}

// enables hardware triggering for an ADC
void ADC::enable_hardware_trigger(ADC_Type *base, bool enable){
	uint32_t index = get_index(base);
	ADC::config[index].hardware_trigger = enable;
	ADC12_EnableHardwareTrigger(base, ADC::config[index].hardware_trigger);
}

// returns the default ADC configuration
void ADC::get_default_config(adc_config_t *config){
	config->function = NULL;
	config->clock_source = kCLOCK_IpSrcFircAsync;
	config->base_config = {kADC12_ReferenceVoltageSourceVref, kADC12_ClockSourceAlt0, kADC12_ClockDivider1, kADC12_Resolution12Bit, 17U, true};
	config->channel_config = {0, false};
	config->hardware_compare_config = {kADC12_HardwareCompareMode0, 0, 0};
	config->hardware_average_mode = kADC12_HardwareAverageDisabled;
	config->offset = 0;
	config->gain = 1;
	config->dma = false;
	config->hardware_trigger = false;
}

// returns the callback function of an ADC
adc_callback_t ADC::get_callback(ADC_Type *base){
	return(ADC::config[get_index(base)].function);
}

// returns the status flags of an ADC
uint32_t ADC::get_base_status_flags(ADC_Type *base){
	return(ADC12_GetStatusFlags(base));
}

// returns the status flags of an ADC channel group
uint32_t ADC::get_channel_status_flags(ADC_Type *base, uint32_t group){
	return(ADC12_GetChannelStatusFlags(base, group));
}

// automatically calibrates an ADC
status_t ADC::calibrate(ADC_Type *base){
	return(ADC12_DoAutoCalibration(base));
}

// reads data from the specified ADC channel
uint32_t ADC::read(ADC_Type *base, uint32_t ch){
	uint32_t index = get_index(base);

	ADC::config[index].channel_config = {ch, false};

	if(!(ADC::config[index].hardware_trigger))
		ADC12_SetChannelConfig(base, 0, &(ADC::config[index].channel_config));

    while(!(ADC12_GetChannelStatusFlags(base, 0) & kADC12_ChannelConversionCompletedFlag)){}

	return(ADC12_GetChannelConversionValue(base, 0));
}

// maps each ADC base to an array index
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
