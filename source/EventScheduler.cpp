/*
 * EventScheduler.cpp
 *
 *  Created on: Oct 24, 2018
 *      Author: oneso
 */

#include <stdio.h>

#include "EventScheduler.h"

using namespace Sys;

EventScheduler::EventScheduler() {

}

void EventScheduler::addService(Service* s, uint32_t interval) {
	if (!s) {
		printf("Attempting to add invalid service\n");
	} else {
		tasks.push_back(new TaskEntry(s, interval));
	}
}

void EventScheduler::update(const float& deltams) {
	for(auto s : tasks) {
		--*s;
		if(s->isCounterZeroed()){
			(*s)();
			s->reset();
		}
	}
}

