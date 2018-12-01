#ifndef ADC_H_
#define ADC_H_

#include "Service.h"
#include "fsl_adc12.h"

namespace BSP {
namespace ADC {

typedef void (*callback)(ADC_Type *, void *, status_t, void *);

struct adc_config {
	callback callbacks[2];
	adc12_config_t port;
	adc12_channel_config_t channel;
	adc12_hardware_compare_config_t hardware_compare;
	adc12_hardware_average_mode_t hardware_average_mode;
	uint32_t offset;
	uint32_t gain;
	bool dma;
	bool hardware_trigger;
};

class ADC final : public StaticService<ADC, const adc_config *> {
public:

    ADC(const adc_config *);

    void tick() override;

    void config_port(ADC_Type *base);
    void config_channel(ADC_Type *base, uint32_t channel);
    void config_hardware_compare(ADC_Type *base);

    void get_default_config(adc_config *config);
    uint32_t get_port_status_flags(ADC_Type *base);
    uint32_t get_channel_status_flags(ADC_Type *base, uint32_t channel);

    void set_offset(ADC_Type *base);
    void set_gain(ADC_Type *base);
    void set_hardware_average(ADC_Type *base);

    void enable_dma(ADC_Type *base);
    void enable_hardware_trigger(ADC_Type *base);

    status_t auto_calibration(ADC_Type *base);
    uint32_t read_channel(ADC_Type *base, uint32_t channel);

private:

    ADC() = default;

    adc_config config;
};

}
}

#endif
