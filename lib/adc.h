#ifndef ADC_H_
#define ADC_H_

#include "Service.h"
#include "Event.h"

namespace BSP {
namespace ADC {

/*! @brief ADC port definition */
enum ADC_port
{
	Port0 = 0U,		/*!< ADC Port 0*/
	Port1 = 1U,		/*!< ADC Port 1*/
	Port2 = 2U,		/*!< ADC Port 2*/
	PortCount = 3U,	/*!< ADC Port Count*/
};

/*! @brief ADC channel definition */
enum ADC_channel
{
	Channel0 = 0U,		/*!< ADC Channel 0*/
	Channel1 = 1U,		/*!< ADC Channel 1*/
	Channel2 = 2U,		/*!< ADC Channel 2*/
	Channel3 = 3U,		/*!< ADC Channel 3*/
	Channel4 = 4U,		/*!< ADC Channel 4*/
	Channel5 = 5U,		/*!< ADC Channel 5*/
	Channel6 = 6U,		/*!< ADC Channel 6*/
	Channel7 = 7U,		/*!< ADC Channel 7*/
	Channel8 = 8U,		/*!< ADC Channel 8*/
	Channel9 = 9U,		/*!< ADC Channel 9*/
	Channel10 = 10U,	/*!< ADC Channel 10*/
	Channel11 = 11U,	/*!< ADC Channel 11*/
	Channel12 = 12U,	/*!< ADC Channel 12*/
	Channel13 = 13U,	/*!< ADC Channel 13*/
	Channel14 = 14U,	/*!< ADC Channel 14*/
	Channel15 = 15U,	/*!< ADC Channel 15*/
	Channel16 = 16U,	/*!< ADC Channel 16*/
	Channel17 = 17U,	/*!< ADC Channel 17*/
	Channel18 = 18U,	/*!< ADC Channel 18*/
	Channel19 = 19U,	/*!< ADC Channel 19*/
	Channel20 = 20U,	/*!< ADC Channel 20*/
	Channel21 = 21U,	/*!< ADC Channel 21*/
	Channel22 = 22U,	/*!< ADC Channel 22*/
	Channel23 = 23U,	/*!< ADC Channel 23*/
	Channel24 = 24U,	/*!< ADC Channel 24*/
	Channel25 = 25U,	/*!< ADC Channel 25*/
	Channel26 = 26U,	/*!< ADC Channel 26*/
	Channel27 = 27U,	/*!< ADC Channel 27*/
	Channel28 = 28U,	/*!< ADC Channel 28*/
	Channel29 = 29U,	/*!< ADC Channel 29*/
	Channel30 = 30U,	/*!< ADC Channel 30*/
	Channel31 = 31U,	/*!< ADC Channel 31*/
	ChanneCount = 32U,	/*!< ADC Channel Count*/
};

/*! @brief ADC port configuration structure */
struct adc_port_config {
    uint32_t reference_voltage_source = 0U;
    uint32_t clock_source = 0U;
    uint32_t clock_divider = 0U;
    uint32_t resolution = 1U;
    uint32_t sample_clock_count = 2U;
    bool enable_continuous_conversion = true;

	adc_port_config() = default;
};

/*! @brief ADC channel configuration structure */
struct adc_channel_config {
    uint32_t channel_number;                    	/*!< Setting the conversion channel number. */
    bool enable_interrupt_on_conversion_completed; 	/*!< Generate a interrupt request once the conversion is completed. */
};

/*! @brief ADC hardware compare configuration structure */
struct adc_hardware_compare_config {
    uint32_t hardware_compare_mode; 	/*!< Select the hardware compare mode. */
    int16_t value1;                     /*!< Setting value1 for hardware compare mode. */
    int16_t value2;                     /*!< Setting value2 for hardware compare mode. */
};

/*! @brief ADC configuration structure */
struct adc_config {
	struct adc_port_config* ADC0_config;
	struct adc_port_config* ADC1_config;
	struct adc_port_config* ADC2_config;
};

class ADC final : public StaticService<ADC, const adc_config*> {
public:
	void tick() override;
	void init() override;

	ADC(const adc_config*);

	/* @brief Configure channel */
	void config_channel(ADC_port port, ADC_channel ch, adc_channel_config *config);

	/* @brief Configure hardware comapre */
	void config_hardware_compare(ADC_port port, adc_hardware_compare_config *config);

	/* @brief Set offset */
	void set_offset(ADC_port port, uint32_t offset);

	/* @brief Set gain */
	void set_gain(ADC_port port, uint32_t gain);

	/* @brief Set hardware average */
	void set_hardware_average(ADC_port port, uint32_t mode);

	/* @brief Enable direct memory access */
	void enable_dma(ADC_port port, bool enable);

	/* @brief Enable hardware trigger */
	void enable_hardware_trigger(ADC_port port, bool enable);

	/* @brief Run auto calibration */
	void auto_calibrate(ADC_port port);

	/* @brief Read channel data */
	uint32_t read(ADC_port port, ADC_channel ch);

private:
	struct adc_port_data;

	adc_port_data* ADC0_config;
	adc_port_data* ADC1_config;
	adc_port_data* ADC2_config;

	ADC() = default;

	/*! @brief Configure port using fsl_adc12 driver. */
	void config_port(ADC_port port, adc_port_config* config);

};

}
}

#endif /* ADC_H_ */
