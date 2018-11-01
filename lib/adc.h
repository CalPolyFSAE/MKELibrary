#ifndef ADC_H_
#define ADC_H_

#include <stdio.h>

#include <Event.h>
#include "Service.h"
#include "fsl_adc12.h"
#include "MKE18F16.h"

/*! @brief ADC port definition */
enum  ADC_port
{
	Port0 = ADC0,	/*!< ADC Port 0*/
	Port1 = ADC1,	/*!< ADC Port 1*/
	Port2 = ADC2,	/*!< ADC Port 2*/
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

class ADC final : public StaticService<ADC> {

public:

	/*! @brief function pointer callback type */
	using ISR_func_ptr = se::Event<int(uint8_t)>;

	/*! @brief ADC interrupt function pointer */
    ISR_func_ptr function[ADC_port::PortCount];

	ADC();

	void tick() override;

	void init() override;

	void config_port(ADC_port port, adc12_config_t *config);

	void config_channel(ADC_port port, ADC_channel ch, adc12_channel_config_t *config);

	void config_hardware_compare(ADC_port port);

	void set_offset(ADC_port port, uint32_t offset);

	void set_gain(ADC_port port, uint32_t gain);

	void set_hardware_average(ADC_port port, adc12_hardware_average_mode_t mode);

	void enable_dma(ADC_port port, bool enable);

	void enable_hardware_trigger(ADC_port port, bool enable);

	void auto_calibrate(ADC_port port);

	uint32_t read(ADC_port port, ADC_channel ch);

private:


};

#endif
