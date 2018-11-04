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
namespace CAN {

static constexpr uint8_t can_number_mbs = 16;
static constexpr uint8_t can_max_msg_size = 8;
using callback_type = se::Event<void(const struct mb_info*)>;

class mb_info {
public:
	enum class mb_transfer_mode {
		TX_single,		// single tx will fire once, then stop
		TX_continuous,	// continuous tx will fire, then wait for next
		RX_single,		// single rx
		RX_continuous,	// continuous rx
		Disabled
	} mode;

	callback_type callback;

	uint32_t 	id_mask;
	uint32_t 	id;
	uint8_t 	data[can_max_msg_size];
	uint8_t 	dlc;
	bool 		remote;
	bool 		extended_id;

	mb_info();

	struct flexcan_frame_t* initialize_frame(struct flexcan_frame_t*);

	/*!
	 * @brief abort any ongoing tx, or rx, clear hardware status, disable mb interrupts, and set mb_info state to disabled
	 */
	void reset();

	/*!
	 * @brief reconfigure the hardware to match mb_info settings
	 */
	void reconfigure_mb(uint8_t mb_id);

	/*!
	 * @brief is this mb currently in a RX or TX mode
	 */
	bool isInUse();
};

struct controller_config {

	enum class clock_source {
		SOSCDIV2_CLK,		// SOSCDIV2_CLK clock source
		BUS_CLK				// Peripheral clock source from PCC
	} clock = clock_source::BUS_CLK;

	uint32_t baudrate = 1000000u;
	bool enableLoopback = false;

	controller_config() = default;
};

struct can_config {
	// input parameters
	// can0 and can1 configs
	//
	struct controller_config* CAN0_config;
	struct controller_config* CAN1_config;
};

class CAN final : public StaticService<CAN, const can_config*> {
public:

	void tick() override;
	void init() override;

	CAN(const can_config*);

	/*!
	 * @brief send a message using any mb
	 */
	bool txMSG(uint8_t bus, uint32_t id, const uint8_t (&data)[can_max_msg_size], uint8_t dlc, bool remote,
			bool extended_id, const callback_type* callback = nullptr);

	/*
	 * rx using selected mb
	 */
	bool rxMSG(uint8_t bus, const mb_info& frame, const callback_type& callback);

	// performance info

	/*!
	 * @brief check how many mbs are in use
	 */
	float getCurrentMBUsage(uint8_t bus);

private:

	struct controller_data;

	// parameters passed in during construction, used during init
	controller_data* CAN0_config;
	controller_data* CAN1_config;

	mb_info mb_can0[can_number_mbs];
	mb_info mb_can1[can_number_mbs];

	CAN() = default;

	/*!
	 * @brief configure mb with rx or tx settings
	 */
	uint8_t setMBConfig(/* MB Settings */);

	void config_controller(uint8_t bus);

};

}
}


#endif /* CAN_H_ */
