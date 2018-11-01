/*
 * can.h
 *
 *  Created on: Oct 24, 2018
 *      Author: oneso
 */

#ifndef CAN_H_
#define CAN_H_

#include "stdint.h"

#include "Service.h"
#include "Event.h"

#include <vector>

namespace BSP {
namespace CAN {

using callback_type = se::Event<void(const struct mb_info*)>;

class mb_info {
public:
	enum class mb_transfer_mode {
		TX,
		RX
	} mode;

	callback_type callback;

	uint32_t 	id;
	uint8_t 	data[8];
	uint8_t 	dlc;
	bool 		remote;
	bool 		extended_id;

	mb_info();

	struct flexcan_frame_t* initialize_frame(struct flexcan_frame_t*);
};

struct controller_config {

	enum class clock_source {
		SOSCDIV2_CLK,		// SOSCDIV2_CLK clock source
		BUS_CLK				// Peripheral clock source from PCC
	} clock = clock_source::BUS_CLK;

	uint32_t baudrate = 1000000;
	bool enableLoopback = false;

	controller_config() = default;
};

class CAN final : public StaticService<CAN, const struct config*> {
public:

	struct config {
		// input parameters
		// can0 and can1 configs
		//
		struct controller_config* CAN0_config;
		struct controller_config* CAN1_config;
	};

	void tick() override;
	void init() override;

	CAN(const config*);

	/*
	 * configure mb with rx or tx settings
	 */
	uint8_t setMBConfig(/* MB Settings */);

	/*
	 * tx using any mb
	 */
	bool txMSG(const mb_info& frame);

	/*
	 *
	 */
	bool txMSG(uint32_t id, const uint8_t data[], uint8_t dlc, bool remote,
			bool extended_id, const callback_type* callback = nullptr);

	/*
	 * rx using selected mb
	 */
	bool rxMSG(const mb_info& frame, const callback_type& callback);

private:

	struct controller_data;

	// parameters passed in during construction, used during init
	controller_data CAN0_config;
	controller_data CAN1_config;

	std::vector<mb_info> mb_can0;
	std::vector<mb_info> mb_can1;

	CAN() = default;

	void config_controller(CAN_Type* base);

};

}
}


#endif /* CAN_H_ */
