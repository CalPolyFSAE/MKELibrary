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

struct TaskFinite : public Task {
private:
    uint32_t delay;
    volatile uint32_t fires;
    volatile bool ready = false;
    volatile bool error = false;
    volatile uint32_t ticks;
    volatile uint32_t overrunTimer = 0;

public:
    TaskFinite() : delay(0), fires(0), ticks(0) {

    }

    TaskFinite(entryPoint_t e, uint16_t id, uint32_t delay, uint32_t fires) : Task(id, e), 
        delay(delay), fires(fires), ticks(delay) {

    }

    void decrementTick() {
        if(ready){
            overrunTimer++;
        }
        if(ticks > 0){
            ticks--;
        }
        else if(ticks == 0){
            if(fires){
                ready = true;
                fires--;
                ticks = delay;
            }
        }
    }

    void operator()(){
        Task::operator()();
        ready = false;
    }

	bool isReady() const {
		return ready;
	}

	uint16_t getTicks() const {
		return ticks;
	}

	uint16_t getDelay() const {
		return delay;
	}

	uint16_t getOverrun() const {
		return overrunTimer;
	}

    void setFires(uint32_t f){
        fires = f;
        if(ticks == 0) ticks = delay;
    }
};

struct TaskPeriodic : public Task {
private:
	uint16_t period;
	uint16_t deadline;
	volatile bool ready = false;
	volatile bool error = false;
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
			if(overrunTimer >= deadline)
				error = true;
		}

		if (ticks > 0) {
			ticks--;
		}

		if (ticks == 0) {
			ready = true;
			ticks = period;
			overrunTimer = 0;
		}
	}

	void operator()() {
		Task::operator ()();
		ready = false;
	}

	bool isReady() const {
		return ready;
	}

	bool isError() const {
		return error;
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
