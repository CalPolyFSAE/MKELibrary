/*
 * EventScheduler.h
 *
 *  Created on: Oct 19, 2018
 *      Author: HB
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <type_traits>
#include <utility>

#include <System/Task.h>
#include <System/Event.h>

namespace System {

template<class TASK_TYPE, uint32_t TASK_COUNT, class R>
class Scheduler {
public:
	static_assert(std::is_base_of<Task, TASK_TYPE>::value, "S must derive from Task.");
	static_assert(TASK_COUNT > 0, "TASK_COUNT must be greater than 0");

	enum class SchedulerState {
		Run,
		Pause,
		Error
	};

	Scheduler();

	void start();
	void pause();

	void onFrame();

	bool addTask(const TASK_TYPE& t);

	uint32_t getTaskCount();

	SchedulerState getRunState();
protected:
	TASK_TYPE taskList[TASK_COUNT] = {};
	uint16_t taskindex = 0;
	volatile SchedulerState runState = SchedulerState::Pause;
};

template<class TASK_TYPE, uint32_t TASK_COUNT, class R>
Scheduler<TASK_TYPE, TASK_COUNT, R>::Scheduler() {

}

template<class TASK_TYPE, uint32_t TASK_COUNT, class R>
void Scheduler<TASK_TYPE, TASK_COUNT, R>::onFrame() {
	if(runState == SchedulerState::Run)
		static_cast<R*>(this)->doNextTask();
}

template<class TASK_TYPE, uint32_t TASK_COUNT, class R>
void Scheduler<TASK_TYPE, TASK_COUNT, R>::start() {
	if(runState != SchedulerState::Error)
		runState = SchedulerState::Run;
}

template<class TASK_TYPE, uint32_t TASK_COUNT, class R>
void Scheduler<TASK_TYPE, TASK_COUNT, R>::pause() {
	if(runState != SchedulerState::Error)
		runState = SchedulerState::Pause;
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

template<class TASK_TYPE, uint32_t TASK_COUNT, class R>
typename Scheduler<TASK_TYPE, TASK_COUNT, R>::SchedulerState
		Scheduler<TASK_TYPE, TASK_COUNT, R>::getRunState() {
	return runState;
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

private:
	using Super = Scheduler<TASK_TYPE, TASK_COUNT, PeriodicScheduler<TASK_TYPE, TASK_COUNT>>;

public:
	static_assert(std::is_base_of<TaskPeriodic, TASK_TYPE>::value, "TASK_TYPE must derive from TaskPeriodic");

	using OnErrorHandler_t = se::Event<void(uint16_t)>;

	PeriodicScheduler();

	/*
	 * @brief do next task if ready. This is called by CRTP runTask function
	 */
	void doNextTask();

	/*
	 * @brief decrement timers
	 */
	void tick();

	/*
	 * @brief set the OnError Callback. Warning: This can be called during tick()
	 *
	 */
	void setOnErrorHandler(OnErrorHandler_t);
private:
	uint16_t currentTask = 0;
	OnErrorHandler_t onErrorHandler;

	void OnError(uint16_t taskId);
};

template<typename TASK_TYPE, uint32_t TASK_COUNT>
PeriodicScheduler<TASK_TYPE, TASK_COUNT>::PeriodicScheduler() {

}

template<typename TASK_TYPE, uint32_t TASK_COUNT>
void PeriodicScheduler<TASK_TYPE, TASK_COUNT>::doNextTask() {
	if (this->runState == Super::SchedulerState::Run) {
		TaskPeriodic* task = nullptr;
		uint32_t taskCount = this->getTaskCount();

		// find task that needs to run
		if (taskCount > 1) {
			for (uint32_t st = (currentTask + 1) % taskCount; st != currentTask;
					st = (st + 1) % taskCount) {
				if (this->taskList[st].isReady()) {
					currentTask = st;
					task = &this->taskList[st];
					break;
				}
			}
		} else {
			if(this->taskList[0].isReady())
				task = &this->taskList[0];
		}

		if (task) {
			(*task)();
		}
	}
}


// this function is called at a fixed rate by external timing ISR
template<typename TASK_TYPE, uint32_t TASK_COUNT>
void PeriodicScheduler<TASK_TYPE, TASK_COUNT>::tick() {
	if (this->runState == Super::SchedulerState::Run) {
		// decrement counters
		uint32_t taskCount = this->getTaskCount();
		for (uint32_t st = 0; st != taskCount; st++) {
			TaskPeriodic& t = this->taskList[st];
			if (t.deadlineIn() == 0 && t.isReady()) {
				this->OnError(t.id);
			}
			t.decrementTick();
		}
	}
}

template<typename TASK_TYPE, uint32_t TASK_COUNT>
void PeriodicScheduler<TASK_TYPE, TASK_COUNT>::setOnErrorHandler(OnErrorHandler_t handler) {
	onErrorHandler = std::move(handler);
}

template<typename TASK_TYPE, uint32_t TASK_COUNT>
void PeriodicScheduler<TASK_TYPE, TASK_COUNT>::OnError(uint16_t taskId) {
	if(this->runState != Super::SchedulerState::Error && !onErrorHandler.isNull()) {
		this->runState = Super::SchedulerState::Error;
		onErrorHandler(taskId);
	}
}

}




#endif /* SCHEDULER_H_ */
