/*
 * SystemTesting.cpp
 *
 *  Created on: Dec 16, 2018
 *      Author: oneso
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

static System::PeriodicScheduler<System::TaskPeriodic, 5> a;

extern "C" {
void SysTick_Handler(void) {
	a.tick();
}
}

static void TestTasks() {

	SysTick_Config(1000000);

	a.addTask(System::TaskPeriodic(Task, 10));
	a.addTask(System::TaskPeriodic(Task1, 15));
	a.addTask(System::TaskPeriodic(Task2, 2));


	while(1)
		a.doNextTask();
}

void TestSys() {
	TestTasks();
}

