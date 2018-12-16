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

	bool addTask(const TASK_TYPE& t);
protected:
	TASK_TYPE taskList[TASK_COUNT] = {};
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
bool Scheduler<TASK_TYPE, TASK_COUNT, R>::addTask(const TASK_TYPE& t) {
	if (taskindex < TASK_COUNT) {
		taskList[taskindex] = t;
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
class PeriodicScheduler : public Scheduler<TASK_TYPE, TASK_COUNT,
		PeriodicScheduler<TASK_TYPE, TASK_COUNT>> {
public:
	static_assert(std::is_base_of<TaskPeriodic, TASK_TYPE>::value, "TASK_TYPE must derive from TaskPeriodic");

	PeriodicScheduler();

	void doNextTask();

	void tick();
private:
	uint16_t currentTask;
};

template<typename TASK_TYPE, int TASK_COUNT>
PeriodicScheduler<TASK_TYPE, TASK_COUNT>::PeriodicScheduler() {

}

// Execute next task if ready
template<typename TASK_TYPE, int TASK_COUNT>
void PeriodicScheduler<TASK_TYPE, TASK_COUNT>::doNextTask() {
	// check that task needs to run
	// get time
	this->taskList[currentTask]();
	// get time, check how long it took to run and record
	currentTask++;
	currentTask %= TASK_COUNT;
}


// this function is called at a fixed rate by external timing
template<typename TASK_TYPE, int TASK_COUNT>
void PeriodicScheduler<TASK_TYPE, TASK_COUNT>::tick() {
	// decrement counters
}

}




#endif /* SCHEDULER_H_ */
