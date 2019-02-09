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
#include <fsl_port.h>
#include <fsl_gpio.h>

static void wait(uint32_t w) {
	volatile uint32_t t = w;
	while(t > 0) {
		t--;
	}
}

static void Task() {
	//printf("Task\n");
	if(GPIO_PinRead(GPIOC, 12U)) {
		GPIO_PinWrite(GPIOC, 12U, 0);
	}else {
		GPIO_PinWrite(GPIOC, 12U, 1);
	}
}

static void Task1() {
	//printf("Task1\n");
	//wait(100);
	if (GPIO_PinRead(GPIOC, 11U)) {
		GPIO_PinWrite(GPIOC, 11U, 0);
	} else {
		GPIO_PinWrite(GPIOC, 11U, 1);
	}
}

static void Task2() {
	//printf("Task2\n");
	if (GPIO_PinRead(GPIOD, 15U)) {
		GPIO_PinWrite(GPIOD, 15U, 0);
	} else {
		GPIO_PinWrite(GPIOD, 15U, 1);
	}
}

using scheduler = System::PeriodicScheduler<System::TaskPeriodic, 5>;
using schedulerfinite = System::SetFireScheduler<System::TaskFinite, 5>;

static void OnErrorHandler(uint16_t task) {
	printf("Error task %u\n", task);
}

static scheduler a;
static schedulerfinite b;

// ISR function for Systick
extern "C" {
void SysTick_Handler(void) {
	b.tick();
}
}

static void TestTasks() {

	port_pin_config_t conf = {};
	conf.driveStrength = kPORT_LowDriveStrength;
	conf.mux = kPORT_MuxAsGpio;
	conf.pullSelect = kPORT_PullDisable;
	conf.lockRegister = kPORT_UnlockRegister;
	conf.passiveFilterEnable = kPORT_PassiveFilterDisable;
	PORT_SetPinConfig(PORTC, 12U, &conf);

	conf.driveStrength = kPORT_LowDriveStrength;
	conf.mux = kPORT_MuxAsGpio;
	conf.pullSelect = kPORT_PullDisable;
	conf.lockRegister = kPORT_UnlockRegister;
	conf.passiveFilterEnable = kPORT_PassiveFilterDisable;
	PORT_SetPinConfig(PORTC, 11U, &conf);

	conf.driveStrength = kPORT_LowDriveStrength;
	conf.mux = kPORT_MuxAsGpio;
	conf.pullSelect = kPORT_PullDisable;
	conf.lockRegister = kPORT_UnlockRegister;
	conf.passiveFilterEnable = kPORT_PassiveFilterDisable;
	PORT_SetPinConfig(PORTC, 13U, &conf);

	GPIO_PinWrite(GPIOC, 12U, 1);
	GPIOC->PDDR |= (1U << 12);

	GPIO_PinWrite(GPIOC, 11U, 1);
	GPIOC->PDDR |= (1U << 11);

	GPIO_PinWrite(GPIOC, 13U, 1);
	GPIOC->PDDR |= (1U << 13);

	SysTick_Config(12000);

	a.setOnErrorHandler(scheduler::OnErrorHandler_t::create<OnErrorHandler>());
	a.addTask(System::TaskPeriodic(Task, 1, 4, 20));
	a.addTask(System::TaskPeriodic(Task1, 2, 50, 30));
	a.addTask(System::TaskPeriodic(Task2, 3, 1000, 30));

	a.start();

	while(1) {
		a.onFrame();
	}
}

void Task3(){
    b.SetTask(1, 3);
}

void TestTasksFinite() {
	port_pin_config_t conf = {};
	conf.driveStrength = kPORT_LowDriveStrength;
	conf.mux = kPORT_MuxAsGpio;
	conf.pullSelect = kPORT_PullDisable;
	conf.lockRegister = kPORT_UnlockRegister;
	conf.passiveFilterEnable = kPORT_PassiveFilterDisable;
	PORT_SetPinConfig(PORTD, 15U, &conf);
	GPIO_PinWrite(GPIOD, 15U, 1);
	GPIOD->PDDR |= (1U << 15);
	SysTick_Config(12000);

    b.addTask(System::TaskFinite(Task2, 1, 500, 0));
    b.addTask(System::TaskFinite(Task3, 2, 3000, 3));
    b.start();
	while(1) {
		b.onFrame();
	}
}

void TestSys() {
	TestTasksFinite();
}

