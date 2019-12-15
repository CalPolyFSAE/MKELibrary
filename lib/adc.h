/*
 * adc.h
 *
 * analog to digital converter driver.
 */

#ifndef ADC_H_
#define ADC_H_

#include "Service.h"
#include "fsl_adc12.h"
#include "fsl_clock.h"

namespace BSP::adc {

// callback function format
typedef void (*adc_callback_t)(ADC_Type *, void *);

// adc driver configuration
typedef struct adc_config {

    // default callback
    adc_callback_t function = NULL;
    
    // default adc clock source
    clock_ip_src_t clock_source = kCLOCK_IpSrcFircAsync;

    // TODO
    adc12_config_t base_config = 
        {kADC12_ReferenceVoltageSourceVref, 
        kADC12_ClockSourceAlt0, 
        kADC12_ClockDivider1, 
        kADC12_Resolution12Bit, 
        17U, 
        true};

    // TODO
    adc12_channel_config_t channel_config = {0, false};

    // TODO
    adc12_hardware_compare_config_t hardware_compare_config = 
        {kADC12_HardwareCompareMode0, 0, 0};

    // TODO
    adc12_hardware_average_mode_t hardware_average_mode = kADC12_HardwareAverageCount16;

    // TODO
    uint32_t offset = 0;

    // TODO
    uint32_t gain = 1;

    // TODO
    bool dma = false;

    // TODO
    bool hardware_trigger = false;

} adc_config_t;

// adc driver
class ADC final : public StaticService<ADC, const adc_config_t *> {
public:

    // adc driver constructor
    // applies configuration settings and calibrates all adcs
    // config: configuration defaults which get applied to all 3 ADCs
    ADC(const adc_config_t * config);

    // configures one adc, including clocks. called for all 3 by the constructor.
    // base: ADC0, ADC1, or ADC2.
    // config: configuration settings
    void config_base(ADC_Type *base, adc12_config_t *config);

    // configures the hardware compare for an ADC
    // base: ADC0, ADC1, ADC2
    // config: TODO
    void config_hardware_compare(ADC_Type *base, adc12_hardware_compare_config_t *config);

    // sets the callback function for an ADC
    // base: ADC0, ADC1, ADC2
    // function: pointer to callback
    void set_callback(ADC_Type *base, adc_callback_t function);

    // sets the offset for an ADC
    // base: ADC0, ADC1, ADC2
    // value: TODO
    void set_offset(ADC_Type *base, uint32_t value);
    
    // sets the gain for an ADC
    // base: ADC0, ADC1, ADC2
    // value: TODO
    void set_gain(ADC_Type *base, uint32_t value);

    // sets the hardware averaging mode for an ADC
    // base: ADC0, ADC1, ADC2
    // mode: TODO
    void set_hardware_average(ADC_Type *base, adc12_hardware_average_mode_t mode);

    // enables DMA for an ADC
    // base: ADC0, ADC1, ADC2
    // enable: TODO
    void enable_dma(ADC_Type *base, bool enable);


    // enables hardware triggering for an ADC
    // base: ADC0, ADC1, ADC2
    // enable: TODO
    void enable_hardware_trigger(ADC_Type *base, bool enable);

    // returns the callback function of one ADC
    // base: ADC0, ADC1, ADC2
    adc_callback_t get_callback(ADC_Type *base);

    // returns the status flags of an ADC
    // base: ADC0, ADC1, ADC2
    uint32_t get_base_status_flags(ADC_Type *base);

    // returns the status flags of an ADC channel group
    // base: ADC0, ADC1, ADC2
    // group: TODO
    uint32_t get_channel_status_flags(ADC_Type *base, uint32_t group);

    // automatically calibrates an ADC
    // base: ADC0, ADC1, ADC2
    status_t calibrate(ADC_Type *base);

    // reads data from the specified ADC channel
    // base: ADC0, ADC1, ADC2
    // ch: TODO
    uint32_t read(ADC_Type *base, uint32_t ch);

private:

    // ???
    ADC() = default;

    // storage for adc configurations
    adc_config_t config[3];

    // maps ADCn to n
    // base: ADC0, ADC1, ADC2
    uint32_t get_index(ADC_Type *base);

};

}

#endif
