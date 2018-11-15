#include "stdint.h"
#include "adc.h"
#include "fsl_adc12.h"

using namespace BSP::ADC;

struct ADC::adc_port_data {
	adc_port_data(const adc_port_config& config) {

	}
};

ADC::ADC(const adc_config* config) :
		ADC0_config(nullptr), ADC1_config(nullptr), ADC2_config(nullptr) {
	assert(config);
	if(config->ADC0_config)
		ADC0_config = new adc_port_data(*(config->ADC0_config));
	if(config->ADC1_config)
		ADC1_config = new adc_port_data(*(config->ADC1_config));
	if(config->ADC2_config)
		ADC2_config = new adc_port_data(*(config->ADC2_config));
}

void ADC::config_port(ADC_port port, adc_port_config* config) {
	ADC_Type *base;
	adc12_config_t adc12_config;

	switch(port) {
		case Port0:
			base = ADC0;
			break;
		case Port1:
			base = ADC1;
			break;
		case Port2:
			base = ADC2;
			break;
		default:
			base = NULL;
	}

	assert(base);
	assert(config);

	ADC12_GetDefaultConfig(&adc12_config);

	adc12_config.clockDivider = (adc12_clock_divider_t)config->clock_divider;
	adc12_config.clockSource = (adc12_clock_source_t)config->clock_source;
	adc12_config.enableContinuousConversion = config->enable_continuous_conversion;
	adc12_config.referenceVoltageSource = (adc12_reference_voltage_source_t)config->reference_voltage_source;
	adc12_config.resolution = (adc12_resolution_t)config->resolution;
	adc12_config.sampleClockCount = config->sample_clock_count;

	ADC12_Init(base, &adc12_config);
}

void ADC::config_channel(ADC_port port, ADC_channel ch, adc_channel_config *config) {
	ADC_Type *base;
	adc12_channel_config_t adc12_channel_config;

	switch(port) {
		case Port0:
			base = ADC0;
			break;
		case Port1:
			base = ADC1;
			break;
		case Port2:
			base = ADC2;
			break;
		default:
			base = NULL;
	}

	assert(base);
	assert(config);

	adc12_channel_config.channelNumber = config->channel_number;
	adc12_channel_config.enableInterruptOnConversionCompleted = config->enable_interrupt_on_conversion_completed;

	ADC12_SetChannelConfig(base, ch, &adc12_channel_config);
}

void ADC::config_hardware_compare(ADC_port port, adc_hardware_compare_config *config) {
	ADC_Type *base;
	adc12_hardware_compare_config_t adc12_hardware_compare_config;

	switch(port) {
		case Port0:
			base = ADC0;
			break;
		case Port1:
			base = ADC1;
			break;
		case Port2:
			base = ADC2;
			break;
		default:
			base = NULL;
	}

	assert(base);
	assert(config);

	adc12_hardware_compare_config.hardwareCompareMode = (adc12_hardware_compare_mode_t)config->hardware_compare_mode;
	adc12_hardware_compare_config.value1 = config->value1;
	adc12_hardware_compare_config.value2 = config->value2;

	ADC12_SetHardwareCompareConfig(base, &adc12_hardware_compare_config);
}

void ADC::set_offset(ADC_port port, uint32_t offset) {
	ADC_Type *base;

	switch(port) {
		case Port0:
			base = ADC0;
			break;
		case Port1:
			base = ADC1;
			break;
		case Port2:
			base = ADC2;
			break;
		default:
			base = NULL;
	}

	assert(base);
	ADC12_SetOffsetValue(base, offset);
}

void ADC::set_gain(ADC_port port, uint32_t gain) {
	ADC_Type *base;

	switch(port) {
		case Port0:
			base = ADC0;
			break;
		case Port1:
			base = ADC1;
			break;
		case Port2:
			base = ADC2;
			break;
		default:
			base = NULL;
	}

	assert(base);
	ADC12_SetGainValue(base, gain);
}

void ADC::set_hardware_average(ADC_port port, uint32_t mode) {
	ADC_Type *base;

	switch(port) {
		case Port0:
			base = ADC0;
			break;
		case Port1:
			base = ADC1;
			break;
		case Port2:
			base = ADC2;
			break;
		default:
			base = NULL;
	}

	assert(base);
	ADC12_SetHardwareAverage(base, (adc12_hardware_average_mode_t)mode);
}

void ADC::enable_dma(ADC_port port, bool enable) {
	ADC_Type *base;

	switch(port) {
		case Port0:
			base = ADC0;
			break;
		case Port1:
			base = ADC1;
			break;
		case Port2:
			base = ADC2;
			break;
		default:
			base = NULL;
	}

	assert(base);
	ADC12_EnableDMA(base, enable);
}

void ADC::enable_hardware_trigger(ADC_port port, bool enable) {
	ADC_Type *base;

	switch(port) {
		case Port0:
			base = ADC0;
			break;
		case Port1:
			base = ADC1;
			break;
		case Port2:
			base = ADC2;
			break;
		default:
			base = NULL;
	}

	assert(base);
	ADC12_EnableHardwareTrigger(base, enable);
}

void ADC::auto_calibrate(ADC_port port) {
	ADC_Type *base;

	switch(port) {
		case Port0:
			base = ADC0;
			break;
		case Port1:
			base = ADC1;
			break;
		case Port2:
			base = ADC2;
			break;
		default:
			base = NULL;
	}

	assert(base);
	ADC12_DoAutoCalibration(base);
}

uint32_t ADC::read(ADC_port port, ADC_channel ch) {
	ADC_Type *base;

	switch(port) {
		case Port0:
			base = ADC0;
			break;
		case Port1:
			base = ADC1;
			break;
		case Port2:
			base = ADC2;
			break;
		default:
			base = NULL;
	}

	assert(base);
	return ADC12_GetChannelConversionValue(base, ch);
}
