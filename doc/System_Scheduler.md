# Scheduler

Board Support Package abstraction for scheduling of repeating or time-considerate tasks.

## Basic Theory

A scheduler is a manager for a finite set of tasks. A task a function like `void f()` which is called after the task's period. The scheduler is initialized with a set of tasks and their periods, and can be turned on and off at any point during runtime.

A basic use case could involve toggling two LEDs at different frequencies. Functions `void toggleRed()` and `void toggleGreen()` would be added to scheduler `s` at the beginning of program execution; after `s.start()` were called, `s` would begin maintaining timers for each task, readying a task for execution every time its timer expired. The task would actually be called (once ready) when `s.onFrame()` were called. An example main loop, then, could be

```
while(1){
    s.onFrame();
}
```

Schedulers can be clocked with any timer, but it is suggested to use the SysTick function. See Basic Use::SysTick.

## Types of Scheduler

### Periodic

A periodic scheduler executes its set of tasks forever, so long as the scheduler is enabled and `onFrame()` is being called. A periodic scheduler should be used to manage any set of tasks which should happen for all of runtime, or for all time while the machine is in a specific state; for example, regularly reading an ADC input, or regularly sending a heartbeat CAN message. 

A periodic scheduler uses periodic tasks. These have associated with them a period and a deadline; the period defines the rate at which the task fires, and the deadline defines how late the task can be before it raises an error. Both values are defined when the task is added to its scheduler and cannot be changed.

### Set-Fire

A set-fire scheduler fires each task a finite number of times. At any time, any task may be set to have any number of remaining fires before it stops firing. Like a periodic scheduler, it is necessary to continue calling `onFrame()` for the scheduler to fire its tasks. A set-fire scheduler should be used to manage tasks which require a real-time delay, but should not execute indefinitely; for example, delaying a message to a peripheral so it has the necessary wakeup time. 

A set-fire scheduler uses finite tasks. These have a period and a remaining fires counter; the period is set when the task is added to the scheduler, but the remaining fires can be updated at any time. 

## Basic Use

### SysTick

The SysTick timer a simple timed interrupt source, and is the easiest way to clock schedulers. Initialize the SysTick timer by calling `SysTick_Config(ticks)`, where `ticks` is the number of system clocks between sequential interrupts. Then add a `tick()` call for each active scheduler to the SysTick interrupt routine. For example:

```
int main(){
    ...
    SysTick_Config(6000);
    ...
}

extern "C" {
void SysTick_Handler(void){
    sch1.tick();
    sch2.tick();
}
}
```

Note that the systick period will be ticks / clock frequency; for example, 6000 ticks at 60MHz yields a 0.1ms systick period.

### Tasks

Tasks are created as either TaskFinite or TaskPeriodic objects, depending on the scheduler in use.

TaskPeriodic is constructed with:
- The task function
- Task ID (any integer)
- Task period, in system ticks
- Task deadline, the number of system ticks the task can be late for before erroring
    This should be a number between 10 and 100, usually. 

```
System::TaskPeriodic(taskfunction, 1, 500, 10);
```

TaskFinite is constructed with:
- The task function
- Task ID
- Task period in system ticks
- Initial number of times to fire task

```
System::TaskFinite(taskfunction, 1, 1000, 4);
```

### Schedulers

Schedulers are templated types which take as template arguments:
- The type of task to be used for that scheduler
- The total number of tasks that scheduler can have

If SysTick is used as a clock source for schedulers, it is convenient to define schedulers as static global variables.

```
// Initialize two schedulers, a 4-task periodic scheduler and a 6-task set-fire scheduler
static System::PeriodicScheduler<System::TaskPeriodic, 4> sch1;
static System::SetFireScheduler<System::TaskFinite, 6> sch2;
```

## Example implementations

### Periodic 

Toggle two GPIO pins at different frequencies:

```
static System::PeriodicScheduler<System::TaskPeriodic, 2> s1;

void task1(){
    gpio::GPIO::toggle(PortD, 15);
}

void task2() {
    gpio::GPIO::toggle(PortD, 16);
}

int main(){
    ...
        
    s1.addTask(System::TaskPeriodic(task1, 1, 500, 30);
    s1.addtask(System::TaskPeriodic(task2, 2, 666, 30);

    s1.start();

    while(1){
        s1.onFrame();
    }
}
```

### Set-Fire

Toggle a single GPIO three times, then repeat thrice:

```
static System::SetFireScheduler<System::TaskFinite, 2> s2;

void task3(){
    gpio::GPIO::toggle(PortD, 15);
}

void task4(){
    // Set task with id 1 to fire 3 times
    s2.SetTask(1, 3);
}

int main(){
    ...

    s2.addTask(System::TaskFinite(task3, 1, 500, 3));
    s2.addTask(System::TaskFinite(task4, 2, 3000, 3));
    s2.start();

    while(1) {
        s2.onFrame();
    }

}
```
