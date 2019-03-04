/*
 * can.h
 *
 *  Created on: Oct 24, 2018
 *      Author: HB
 *
 *	Description:
 *		This library is a wrapper around the supplied CAN driver. It allows for
 *		another level of abstraction away from the driver. The wrapper tracks
 *		CAN hardware configuration and the state of all message buffers. There
 *		is no need for user to track which message buffers are in use.
 *
 *		This library is capable of configuring CAN hardware and sending messages on
 *		a per request basis. It will not convert large sets of data into data streams.
 *		Any data object over 8 bytes must be split manually. There is also currently
 *		no buffer for incoming CAN messages. Messages need to be read out of RX message
 *		buffer before reseting.
 *
 *	Planned Features:
 *		1. Errors reported through exceptions.
 *			- std::exception
 *				- can_error
 *					- can_invalid_config_error
 *					- can_hardware_error
 *					- can_unknown_error
 *		2. RX message buffers of configurable size.
 *		3. Data streaming.
 */

#ifndef CANDRV_H_
#define CANDRV_H_

#include "stdint.h"

#include "Service.h"
#include "Event.h"

namespace BSP {
namespace can {

template<typename T>
T byteSwap(const T& val);

template<>
uint16_t byteSwap<uint16_t>(const uint16_t& val);

template<>
uint32_t byteSwap<uint32_t>(const uint32_t& val);

static constexpr uint8_t number_MBs 		= 16;
static constexpr uint8_t max_msg_size 		= 8;
static constexpr uint8_t number_controllers = 2;

enum class Controller {
	bus_CAN0,
	bus_CAN1
};

using callback_type = se::Event<void(const uint8_t*, uint8_t)>;

// info for individual message buffers in CAN controller
// warning: this cannot be constructed until after can controller is constructed.
class MB_info {
public:

	enum class MB_transfer_mode {
		TX_single,		// single tx will fire once, then stop
		TX_continuous,	// continuous tx will fire, then wait for next
		RX_single,		// single rx
		RX_continuous,	// continuous rx
		disabled
	};

private:
	callback_type callback;
	MB_transfer_mode mode = MB_transfer_mode::disabled;

	uint8_t mb_id;

	// wrapper struct for the fsl_can flexcan_frame_t type
	//TODO: remove public
public:
	struct Config_data;
	Config_data* config_data = nullptr;

	struct Frame_data;
	Frame_data* frame_data = nullptr;

	volatile bool completed = false;

	// count number of callback events missed. When there is more than one msg RX per update
	volatile uint32_t overrun_count = 0;

	// TODO RX message buffers should go in here
	// array of frame data
public:

	MB_info() = default;
	MB_info(const Config_data* conf, uint8_t mb_id);
	~MB_info();

	/*!
	 * @brief abort any ongoing tx, or rx, clear hardware status, disable mb interrupts, and set mb_info state to disabled
	 */
	void reset(bool disable = false);

	/*!
	 * @brief Check if completed flag is set, then fire callback if valid and clear flag
	 */
	void do_callback();

	/*!
	 * @brief Check if the last event has not been serviced. Set completed flag.
	 */
	inline void ISR_set_completed() {
		if(completed)
			++overrun_count;// increment error
		completed = true;
	}

	/*!
	 * @brief set the current callback for completion
	 */
	inline void set_callback(const callback_type& callback) {
		this->callback = callback;
	}

	/*!
	 * @brief configure the frame data for TX modes
	 */
	bool set_frame_tx(uint32_t id, bool extended_id, bool tx_continuous);

	bool tx_data(const uint8_t (&data)[max_msg_size], uint8_t dlc, bool remote = false);

	/*!
	 * @brief configure the frame data for RX modes
	 */
	bool set_frame_rx(uint32_t id, uint8_t dlc, bool remote, bool extended_id, uint32_t mask);

	/*!
	 * @brief read last frame received
	 */
	void rx_data();

	/*!
	 * @brief is this mb currently in one of the RX or TX modes
	 */
	inline bool is_in_use() const {
		return mode != MB_transfer_mode::disabled;
	}
};

struct Controller_config {

	enum class Clock_source {
		SOSCDIV2_clk,		// SOSCDIV2_CLK clock source
		BUS_clk				// Peripheral clock source from PCC
	} clock = Clock_source::BUS_clk;

	uint32_t baudrate = 1000000u;
	bool enableLoopback = false;

	Controller_config() = default;
};

struct can_config {
	// input parameters
	// can0 and can1 configs
	//
	Controller_config* CAN0_config = nullptr;
	Controller_config* CAN1_config = nullptr;
};

class CAN_drv final : public StaticService<CAN_drv, const can_config*> {
public:
	friend MB_info;

	void tick() override;
	void init() override;

	CAN_drv(const can_config*);

	/*!
	 * @brief send a message using any mb
	 */
	bool tx_msg(Controller bus, uint32_t id,
			const uint8_t (&data)[max_msg_size], uint8_t dlc, bool remote,
			bool extended_id, callback_type callback);

	/*
	 * rx using selected mb
	 */
	bool rx_msg(Controller bus, uint32_t id, bool extended_id, uint8_t dlc,
			bool remote, uint32_t mask, callback_type callback);


	// performance info

	/*!
	 * @brief get how many mbs are in use
	 */
	float get_metric_current_MB_usage(Controller bus) {
		return 0.0f;
	}

	/*!
	 * @brief get current RX error counter
	 */
	uint32_t get_metric_current_CAN_RX_error(Controller bus) {
		return 0;
	}

	/*!
	 * @brief get current TX error counter
	 */
	uint32_t get_metric_current_CAN_TX_error(Controller bus) {
		return 0;
	}

	/*!
	 * @brief Do not call. This is for the fsl_can driver interrupt callback.
	 */
	void ISR_CAN_driver_RX(Controller bus, uint32_t result);

	/*!
	 * @brief Do not call. This is for the fsl_can driver interrupt callback.
	 */
	void ISR_CAN_driver_TX(Controller bus, uint32_t result);


private:

	struct Controller_data;

	// parameters passed in during construction, used during init
	Controller_data* CAN0_config;
	Controller_data* CAN1_config;

	MB_info mb_can0[number_MBs];
	MB_info mb_can1[number_MBs];

	CAN_drv() = default;

	/*!
	 * @brief configure mb with rx or tx settings
	 */
	uint8_t set_MB_config(/* MB Settings */);

	/*!
	 * @brief Configure controller hardware using fsl_can driver. Gets handle for fsl_can driver.
	 */
	void config_controller(Controller_data* config, Controller bus);


	MB_info* get_free_mb_info(Controller bus);
};

}
}


#endif /* CANDRV_H_ */
