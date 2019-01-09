/*
 * Task.h
 *
 *  Created on: Nov 22, 2018
 *      Author: HB
 */

#ifndef TASK_H_
#define TASK_H_

#include <System/System.h>

#include <stdint.h>

namespace System {

inline void emptyTask(void) {
	return;
}

struct TaskBase {
	uint16_t id;
	TaskBase(const TaskBase&) = default;
	TaskBase(uint16_t id) : id(id) {

	}
};

struct Task : public TaskBase {
protected:
	entryPoint_t entry = emptyTask;

public:
	Task() : TaskBase(0) {
	}

	Task(const Task& other) = default;
	Task(uint16_t id, entryPoint_t entry) :
		TaskBase(id), entry(entry) {
	}

	void operator()(){
		entry();
	}

};

struct TaskPeriodic : public Task {
private:
	uint16_t period;
	uint16_t deadline;
	volatile bool ready = false;
	volatile uint16_t ticks;// timer
	volatile int16_t overrunTimer = 0;// timer, how long since task became active

public:
	TaskPeriodic() : period(0), deadline(0), ticks(0) {

	}

	TaskPeriodic(entryPoint_t e, uint16_t id, uint16_t period, uint16_t deadline) : Task(id, e), period(period), deadline(deadline), ticks(period) {

	}

	TaskPeriodic(const TaskPeriodic& other) = default;

	/*
	 * @brief separate thread (ISR) decrements timer.
	 */
	void decrementTick() {
		if (ready) {
			overrunTimer++;
		}

		if (ticks > 0) {
			ticks--;
		} else if (ticks == 0) {
			ready = true;
			ticks = period;
		}
	}

	void operator()() {
		Task::operator ()();
		reset();
	}

	void reset() {
		ready = false;
		overrunTimer = 0;
	}

	bool isReady() {
		return ready;
	}

	uint16_t getTicks() const {
		return ticks;
	}

	uint16_t getPeriod() const {
		return period;
	}

	uint16_t getOverrun() const {
		return overrunTimer;
	}

	uint16_t deadlineIn() const {
		if(overrunTimer < deadline)
			return deadline - overrunTimer;
		return 0;
	}
};

}


#endif /* TASK_H_ */