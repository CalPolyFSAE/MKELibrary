/*
 * objects.h
 *
 *  Created on: Mar 5, 2019
 *      Author: HB
 *
 * 	Description structures for hardware
 */

#ifndef OBJECTS_H_
#define OBJECTS_H_


#include "MKE18F16.h"
#include "fsl_clock.h"

namespace can {
	class can_api;
}

// CAN
struct can_t {
	clock_ip_name_t 	clock;
	IRQn				irqn;
	IRQn				irqn_err;
	CAN_Type* 			base;
	can::can_api* 		driver;
};

/* Can config */
struct can_config_t {
	uint32_t 			baud			= 1000000;
	bool 				enableLoopback	= false;
	bool 				enableSelfWake	= false;
};


/* device descriptors */

/* CAN */
extern can_t CAN0Description;

extern can_t CAN1Description;
/* CAN end */

#endif /* OBJECTS_H_ */
