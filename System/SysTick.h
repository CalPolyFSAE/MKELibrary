/*
 * SysTick.h
 *
 *  Created on: Feb 2, 2019
 *      Author: oneso
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <System/NoCopy.h>
#include <System/Event.h>

class SysTick : public NoCopy<SysTick> {
public:

	SysTick();

	/** Creates CAN message with specific content.
	 *
	 *  @param tickEvent 	function to tick
	 */
	void attach(se::Event<void(void)> tickEvent);

protected:


};


#endif /* SYSTICK_H_ */
