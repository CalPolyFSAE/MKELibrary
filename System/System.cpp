/*
 * System.cpp
 *
 *  Created on: Dec 16, 2018
 *      Author: oneso
 */

#include <System/System.h>

using namespace System;

static ExeState sysState = ExeState::Init;

ExeState getState() {
	return sysState;
}
