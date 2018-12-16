/*
 * SystemTesting.cpp
 *
 *  Created on: Dec 16, 2018
 *      Author: oneso
 */
#include <stdio.h>

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

static void TestTasks() {
	System::PeriodicScheduler<System::TaskPeriodic, 3> a;

	a.addTask(System::TaskPeriodic(Task, 1));
	a.addTask(System::TaskPeriodic(Task1, 1));
	a.addTask(System::TaskPeriodic(Task2, 1));


	for(uint16_t i = 0; i < 5; ++i)
		a.doNextTask();
}

void TestSys() {
	TestTasks();
}

