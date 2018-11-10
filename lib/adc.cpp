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
