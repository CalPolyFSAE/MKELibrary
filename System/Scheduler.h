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

template<class TASK_TYPE, uint32_t TASK_COUNT, class R>
class Scheduler {
public:
	static_assert(std::is_base_of<Task, TASK_TYPE>::value, "S must derive from Task.");
	static_assert(TASK_COUNT > 0, "TASK_COUNT must be greater than 0");

	Scheduler();

	void runTask();

	bool addTask(const TASK_TYPE& t);

	uint32_t getTaskCount();
protected:
	TASK_TYPE taskList[TASK_COUNT] = {};
	uint16_t taskindex = 0;
};

template<class TASK_TYPE, uint32_t TASK_COUNT, class R>
Scheduler<TASK_TYPE, TASK_COUNT, R>::Scheduler() {

}

template<class TASK_TYPE, uint32_t TASK_COUNT, class R>
void Scheduler<TASK_TYPE, TASK_COUNT, R>::runTask() {
	static_cast<R>(*this).doNextTask();
}

template<class TASK_TYPE, uint32_t TASK_COUNT, class R>
bool Scheduler<TASK_TYPE, TASK_COUNT, R>::addTask(const TASK_TYPE& t) {
	if (taskindex < TASK_COUNT) {
		taskList[taskindex] = t;
		taskindex++;
		return true;
	} else
		return false;
}

template<class TASK_TYPE, uint32_t TASK_COUNT, class R>
uint32_t Scheduler<TASK_TYPE, TASK_COUNT, R>::getTaskCount() {
	return taskindex;
}

////////Scheduler implementations//////////////

template<typename TASK_TYPE, uint32_t TASK_COUNT>
class StateScheduler: public Scheduler<TASK_TYPE, TASK_COUNT,
		StateScheduler<TASK_TYPE, TASK_COUNT>> {
public:
	void doNextTask() {

	}
};

/*
 * @brief Non-preemptive Periodic Task Scheduler
 */
template<typename TASK_TYPE, uint32_t TASK_COUNT>
class PeriodicScheduler : public Scheduler<TASK_TYPE, TASK_COUNT,
		PeriodicScheduler<TASK_TYPE, TASK_COUNT>> {
public:
	static_assert(std::is_base_of<TaskPeriodic, TASK_TYPE>::value, "TASK_TYPE must derive from TaskPeriodic");

	PeriodicScheduler();

	void doNextTask();

	/*
	 * @brief decrement timers
	 */
	void tick();
private:
	uint16_t currentTask = 0;
};

template<typename TASK_TYPE, uint32_t TASK_COUNT>
PeriodicScheduler<TASK_TYPE, TASK_COUNT>::PeriodicScheduler() {

}

// Execute next task if ready
template<typename TASK_TYPE, uint32_t TASK_COUNT>
void PeriodicScheduler<TASK_TYPE, TASK_COUNT>::doNextTask() {
	// check that task needs to run
	TaskPeriodic* task = nullptr;
	uint32_t taskCount = this->getTaskCount();
	for(uint32_t st = (currentTask + 1) % taskCount; st != currentTask; st = (st + 1) % taskCount) {
		if(this->taskList[st].getTicks() == 0) {
			currentTask = st;
			task = &this->taskList[st];
			break;
		}
	}
	// TODO get time
	if(task) {
		(*task)();
		task->resetTimer();
	}
	// TODO get time, check how long it took to run and record
}


// this function is called at a fixed rate by external timing ISR
template<typename TASK_TYPE, uint32_t TASK_COUNT>
void PeriodicScheduler<TASK_TYPE, TASK_COUNT>::tick() {
	// decrement counters
	for(TaskPeriodic& t : this->taskList) {
		t.decrementTick();
	}
}

}




#endif /* SCHEDULER_H_ */
