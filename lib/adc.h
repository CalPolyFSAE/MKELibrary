#ifndef ADC_H_
#define ADC_H_

#include "Service.h"
#include "fsl_adc12.h"

namespace BSP {
namespace ADC {

typedef void (*adc_callback_t)(ADC_Type *, void *, status_t, void *);

typedef struct adc_config {
	adc_callback_t function;
	adc12_config_t port;
	adc12_channel_config_t channel;
	adc12_hardware_compare_config_t hardware_compare;
	adc12_hardware_average_mode_t hardware_average_mode;
	uint32_t offset;
	uint32_t gain;
	bool dma;
	bool hardware_trigger;
} adc_config_t;

class ADC final : public StaticService<ADC, const adc_config_t *> {
public:

    ADC(const adc_config_t *);

    void tick() override;

    void config_port(ADC_Type *base, adc12_config_t *config);
    void config_channel(ADC_Type *base, uint32_t channel, adc12_channel_config_t *config);
    void config_hardware_compare(ADC_Type *base, adc12_hardware_compare_config_t *config);

    void set_callback(ADC_Type *base, adc_callback_t function);
    void set_offset(ADC_Type *base, uint32_t value);
    void set_gain(ADC_Type *base, uint32_t value);
    void set_hardware_average(ADC_Type *base, adc12_hardware_average_mode_t mode);

    void enable_dma(ADC_Type *base, bool enable);
    void enable_hardware_trigger(ADC_Type *base, bool enable);

    void get_default_config(adc_config *config);
    uint32_t get_port_status_flags(ADC_Type *base);
    uint32_t get_channel_status_flags(ADC_Type *base, uint32_t channel);

    status_t auto_calibration(ADC_Type *base);
    uint32_t read_channel(ADC_Type *base, uint32_t channel);

private:

    ADC() = default;

    adc_config_t config[3];

    adc_callback_t get_callback(ADC_Type *base);
    uint32_t get_index(ADC_Type *base);
};

}
}

#endif
