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

namespace BSP {

class CAN final : public StaticService<CAN, const struct config*> {
public:

	struct config {
		// input parameters
		// flags for can0 and can1 base
		//
	};

	struct controller_config {
		uint32_t baudrate;
	};

	struct mb_config {

	};

	struct mb_frame {
		uint16_t id;
		uint8_t data[8];
		uint8_t dlc;
	};

	using callback_type = se::Event<void(const mb_config&)>;

	void tick() override {}
	void init() override {}

	CAN(const config*);

	/*
	 * configure mb with rx or tx settings
	 */
	uint8_t setMBConfig(const mb_config*);

	/*
	 * tx using selected mb
	 */
	bool txMSG(const mb_frame*);

	/*
	 * rx using selected mb
	 */
	bool rxMSG(const mb_frame*, const callback_type& callback);

private:
	CAN() = default;


};

}


#endif /* CAN_H_ */
