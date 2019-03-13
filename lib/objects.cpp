/*
 * objects.cpp
 *
 *  Created on: Mar 11, 2019
 *      Author: HB
 */

#include "objects.h"

/* device descriptors */

/* CAN */
can_t CAN0Description {
	kCLOCK_Flexcan0,
	CAN0_ORed_Message_buffer_IRQn,
	CAN0_Error_IRQn,
	CAN0,
	0
};

can_t CAN1Description {
	kCLOCK_Flexcan1,
	CAN1_ORed_Message_buffer_IRQn,
	CAN1_Error_IRQn,
	CAN1,
	0
};

/* CAN end */




