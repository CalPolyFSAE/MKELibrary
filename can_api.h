/*
 * can_api.h
 *
 *  Created on: Mar 4, 2019
 *      Author: HB
 *
 *  Can hardware simple api.
 *  Usage: 	Construct can_api with can_t static object pointer. can_t
 *  		describes clock connection and irq properties. Use init function to
 *  		initialize hardware.
 *  		This driver is experimental.
 *
 *	Version: 0.1
 */

#ifndef CAN_API_H_
#define CAN_API_H_

#include <stdint.h>

#include "objects.h"

namespace can {

/*
 * CAN Id format
 */
enum class CanFormat : uint8_t {
	Standard	= 0,
	Extended	= 1
};

enum class CanFrameType : uint8_t {
	Data		= 0,
	Remote		= 1
};

enum class CanIrqType : uint16_t {
	TX			= 0x0001,
	RX			= 0x0002,
	ERROR  		= 0x0004,
	OVERRUN		= 0x0008,
	WAKEUP		= 0x0010,
	PASSIVE		= 0x0020,
	ARB			= 0x0040,
	BUS			= 0x0080,
	READY		= 0x0100
};

/*
 * Data for a single CAN message
 */
struct CANMessage {
	uint32_t		id;
	union {
		uint8_t		data[8];
		uint32_t	dataWord[2];
	};
	uint8_t 		len;
	CanFormat		format;
	CanFrameType	type;
};

using CanIrqHandlerType = void(*)(CanIrqType, class can_api*, int32_t handle);

// TODO delete all copy constructors and operators
class can_api {
public:
	/*
	 * can_t resource to use
	 */
	can_api() = delete;
	explicit can_api(can_t* obj);
	virtual ~can_api();

	/*!
	 * @brief Initialize the Can controller hardware.
	 *
	 * @param config config struct reference
	 */
	void init(const can_config_t& config);

	/*!
	 * @brief Configure Can controller hardware to work with bus freq
	 *
	 * @param freq bus bit frequency
	 */
	void setFrequency(uint32_t freq);

	/*!
	 * @brief Write a Can message to bus non-blocking.
	 * Sends on first available mb.
	 * returns 0 on success
	 *
	 * @param msg Reference to CANMessage structure with frame data.
	 *
	 * @retval -1 could not find mb to transmit on
	 * @retval 0 success
	 */
	int32_t write(const CANMessage& msg);

	/*!
	 * @brief Read message from handle, or from first full mb
	 *
	 * @param msg reference to message structure to write frame data into
	 * @param handle handle for message buffer to read from. If 0, first full mb will be used.
	 *
	 * @retval valid (non negative) handle on success
	 * @retval -1 could not read on handle
	 */
	int32_t read(CANMessage& msg, int32_t handle = 0);

	/*!
	 * @brief get rx error count
	 * @retval rx error count
	 */
	uint8_t rerror();

	/*!
	 * @brief get tx error count
	 * @retval rx error count
	 */
	uint8_t terror();

	/*
	 * Set CAN controller mode
	 */
	// int32_t mode();

	/*!
	 * @brief Create a filter for incoming messages on handle.
	 * Use the returned handle to read message received by this mb.
	 *
	 * @param id CAN id to receive
	 * @param mask filter mask for id
	 * @param format Extended or Standard id
	 * @param type Data or Remote frame
	 * @param filterType should incoming messages on id be filtered by frame type
	 * @param handle to filter on
	 *
	 * @retval -1 filter could not be created
	 * @retval handle filter was created on handle. Pass handle to read to get messages received on handle
	 */
	int32_t filter(uint32_t id, uint32_t mask, CanFormat format, CanFrameType type, bool filterType = true, int32_t handle = 0);

	/*!
	 * @brief Check if handle is currently available.
	 *
	 * @param valid handle
	 *
	 * @retval true handle is in use
	 */
	bool isInUse(int32_t handle);

	/* close a filter. Returns 0 on success */
	int32_t closeFilter(int32_t handle);

	/*
	 * Initialize and enable the irq handler
	 */
	void can_irq_init(CanIrqHandlerType handler);

	/*
	 * Enable irq type
	 */
	void can_irq_set(CanIrqType irq, bool enabled);

	/*
	 * Disable irq handler and reset enabled interrupts
	 */
	void can_irq_free();

	/* can irq handler */
	void can_irq_handler();

protected:
	can_t* obj;
	CanIrqHandlerType handler;

	/* irq handler enabled interrupts for callback */
	uint16_t interruptMask;

	/*
	 * Enable and disable the CAN controller.
	 */
	void enable(bool enabled);

	/*
	 * Enable and disable freeze mode. Freeze mode allows for
	 * modification of configuration registers and message buffers
	 */
	void freeze(bool enabled);

	/*
	 * Reset the CAN controller hardware to a known state
	 */
	void reset();

	/*
	 * Configure clock config for baud
	 */
	void setupClock(uint32_t baud);

	/* Find MB with code field matching given code with mask */
	int16_t findFirstMBWithCode(uint8_t code, uint8_t mask);

	/* Find first MB with inactive TX or inactive RX code */
	int16_t findFirstInactiveMB();

	/*!
	 * @brief read the frame data out of full mb
	 *
	 * @param mbid message buffer index
	 * @param msg CANMessage struct to copy message into
	 *
	 * @retval -1 mb was not full
	 * @retval 0 mb was full
	 */
	int8_t readMB(uint16_t mbid, CANMessage& msg);

	/*
	 * Write abort code into message buffer and wait for completion.
	 * Clean all fields, clear interrupt flag and mask. Returns cs code for abort or tx.
	 * This does not reset rx filter values.
	 */
	uint8_t clearMB(uint8_t mbid);

	/* Enable or disable mb interrupts */
	void setMBInterrupt(uint16_t mbid, bool enabled);

	/* config mb for tx */
	void configTxMB(uint16_t mbid, const CANMessage& msg);

	/* config mb for rx */
	void configRxMB(uint16_t mbid, uint32_t id, uint32_t idmask,
			CanFormat format, CanFrameType type, bool filtertype,
			bool filterformat);
};


}


#endif /* CAN_API_H_ */
