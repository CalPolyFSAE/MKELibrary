/*
 * SystemTesting.cpp
 *
 *  Created on: Dec 16, 2018
 *      Author: HB
 */
#include <stdio.h>
#include <MKE18F16.h>

#include <System/System.h>
#include <System/Scheduler.h>

static void Task() {
	printf("Task\n");
}

static void Task1() {
	printf("Task1\n");
}

static void Task2() {
	printf("Task2\n");
}

using scheduler = System::PeriodicScheduler<System::TaskPeriodic, 5>;

static void OnErrorHandler(uint16_t task) {
	printf("Error %u\n", task);
}

static scheduler a;

// ISR function for Systick
extern "C" {
void SysTick_Handler(void) {
	a.tick();
}
}

static void TestTasks() {

	SysTick_Config(500000);

	a.setOnErrorHandler(scheduler::OnErrorHandler_t::create<OnErrorHandler>());
	a.addTask(System::TaskPeriodic(Task, 1, 10, 10));
	a.addTask(System::TaskPeriodic(Task1, 2, 15, 10));
	a.addTask(System::TaskPeriodic(Task2, 3, 10, 10));

	a.start();

	while(1)
		a.onFrame();
}

void TestSys() {
	TestTasks();
}

