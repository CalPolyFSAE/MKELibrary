/*
 * System.cpp
 *
 *  Created on: Dec 16, 2018
 *      Author: HB
 */

#include <System/System.h>

using namespace System;

static ExeState sysState = ExeState::Init;

ExeState getState() {
	return sysState;
}
