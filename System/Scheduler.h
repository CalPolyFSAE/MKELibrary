/*
 * EventScheduler.h
 *
 *  Created on: Oct 19, 2018
 *      Author: oneso
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <type_traits>

#include <System/Task.h>

namespace System {

template<class TASK_TYPE, int TASK_COUNT, class R>
class Scheduler {
public:
	static_assert(std::is_base_of<Task, TASK_TYPE>::value, "S must derive from Task.");
	static_assert(TASK_COUNT > 0, "TASK_COUNT must be greater than 0");

	Scheduler();

	void runTask();

	bool addTask(TASK_TYPE& t);
protected:
	TASK_TYPE tasks[TASK_COUNT] = {};
	uint16_t taskindex = 0;
};

template<class TASK_TYPE, int TASK_COUNT, class R>
Scheduler<TASK_TYPE, TASK_COUNT, R>::Scheduler() {

}

template<class TASK_TYPE, int TASK_COUNT, class R>
void Scheduler<TASK_TYPE, TASK_COUNT, R>::runTask() {
	static_cast<R>(*this).doNextTask();
}

template<class TASK_TYPE, int TASK_COUNT, class R>
bool Scheduler<TASK_TYPE, TASK_COUNT, R>::addTask(TASK_TYPE& t) {
	if (taskindex < TASK_COUNT) {
		tasks[taskindex] = t;
		taskindex++;
		return true;
	} else
		return false;
}

////////Scheduler implementations//////////////

template<typename TASK_TYPE, int TASK_COUNT>
class StateScheduler: public Scheduler<TASK_TYPE, TASK_COUNT,
		StateScheduler<TASK_TYPE, TASK_COUNT>> {
public:
	void doNextTask() {

	}
};

template<typename TASK_TYPE, int TASK_COUNT>
class PeriodicScheduler: public Scheduler<TASK_TYPE, TASK_COUNT,
		PeriodicScheduler<TASK_TYPE, TASK_COUNT>> {
public:
	void doNextTask() {

	}
private:
	uint16_t currentTask;
};

}




#endif /* SCHEDULER_H_ */
