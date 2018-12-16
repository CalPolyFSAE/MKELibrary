/*
 * System.h
 *
 *  Created on: Nov 23, 2018
 *      Author: oneso
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>

namespace System {

using entryPoint_t = void(*)(void);

enum class ExeState : uint8_t {
	Init,
	Run
};

ExeState getState();

}


#endif /* SYSTEM_H_ */
