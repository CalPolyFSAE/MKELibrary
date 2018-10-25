/*
 * EventScheduler.h
 *
 *  Created on: Oct 19, 2018
 *      Author: oneso
 */

#ifndef EVENTSCHEDULER_H_
#define EVENTSCHEDULER_H_

#include <vector>

#include "Service.h"

namespace Sys {

class EventScheduler {
public:

	EventScheduler();

	void addService(Service* s, uint32_t interval);

	void update(const float& deltams);

	struct TaskEntry {
	private:
		Service *const task;
		const uint32_t Interval;
		uint32_t count;

	public:
		TaskEntry(Service* service, uint32_t Interval) :
				task(service), Interval(Interval), count(Interval) {
		}

		void reset() {
			count = Interval;
		}

		uint32_t operator--() {
			if(count > 0)
				--count;
			return count;
		}

		void operator()() {
			if(task)
				task->tick();
		}

		bool isCounterZeroed() {
			if(count == 0)
				return true;
			return false;
		}
	};
private:
	std::vector<TaskEntry*> tasks;
};

}


#endif /* EVENTSCHEDULER_H_ */
