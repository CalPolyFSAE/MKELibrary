/*
 * can.cpp
 *
 *  Created on: Oct 27, 2018
 *      Author: oneso
 */

#include "can.h"

using namespace BSP;

CAN::CAN(const config* conf){

}

void CAN::tick() {
	// flag check
}

void CAN::init() {
	// HW init
}

bool CAN::txMSG(const mb_frame&) {
	return false;
}

bool CAN::rxMSG(const mb_frame&, const callback_type&) {
	return false;
}

