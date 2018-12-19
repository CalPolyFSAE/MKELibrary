/*
 * Task.h
 *
 *  Created on: Nov 22, 2018
 *      Author: oneso
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
	entryPoint_t entry = emptyTask;

	Task() : TaskBase(0) {
	}

	Task(entryPoint_t e) : TaskBase(0), entry(e) {

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
	volatile uint16_t ticks;// timer

public:
	TaskPeriodic() : period(0), ticks(0) {

	}

	TaskPeriodic(entryPoint_t e, uint16_t period) : Task(e), period(period), ticks(0) {

	}

	TaskPeriodic(const TaskPeriodic& other) = default;

	/*
	 * @brief separate thread (ISR) decrements timer.
	 */
	void decrementTick() {
		if(ticks > 0)
			ticks--;
	}

	void resetTimer() {
		ticks = period;
	}

	uint16_t getTicks() {
		return ticks;
	}

	uint16_t getPeriod() const {
		return period;
	}
};

}


#endif /* TASK_H_ */
