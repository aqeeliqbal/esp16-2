﻿/*****************************************************************************
Created by group 3.
Erik Martín Campaña
Elisabeth Eriksson
Tamilkumari Padmanathan
Shijie Zhu
                             Test Cases 1
This case is used to test the ability of dynamical task-creating. In the func-
tion Task 1, Task 2 will be created. Both tasks need to take Semaphore 1, while 
Task 2 has higher priority than task1. So when task 2 is created, according to
IPCP algorithm, Ceil(Semaphore 1) should be updated dynamically and at the same
time prio(task1) == prio(task2).

Passing the test: task 2 works, and Led 1 should light up and blink.

WARNING: Only System with Round Robin between tasks with the same priority can
make task 2 work. 
*****************************************************************************/

//Standard library includes
#include <avr32/io.h>

//Kernel includes
#include "kernel/rosa_ker.h"
//#include "kernel/rosa_extended_ker.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"

//Include configuration
#include "rosa_config.h"

#include "test.h"

//Data blocks for the tasks
#define T1_PRIORITY	  (0+2)
static int t1_stack[0x040];

#define T2_PRIORITY	  (0+4)
static int t2_stack[0x040];

//Semaphore & Array;
static semHandle Semaphore1  ;
static semHandle Task1_Array[1];
static semHandle Task2_Array[1];

tcbHandle t1task2;



void task2_1(void){
	ticktime StartTime = 0;
	ticktime Period    = 200;
	while(1){
		while(ROSA_semaphoreTake(Semaphore1));
		ledOn(LED1_GPIO);
		ROSA_taskDelay(100);
		ledOff(LED1_GPIO);
		ROSA_semaphoreGive(Semaphore1);
		ROSA_taskDelayUntil(&StartTime,Period);
	}
}

void task1_1(void){
	while(1){
		while(ROSA_semaphoreTake(Semaphore1));
		/*create task2 here dynamically*/
		ROSA_tcbCreate(&t1task2, "tsk2", task2_1, t2_stack, 0x040,T2_PRIORITY,NULL,Task2_Array,1);
		ledOn(LED0_GPIO);
		ROSA_taskDelay(2000);
		ledOff(LED0_GPIO);
		ROSA_semaphoreGive(Semaphore1);
	}
}

/*************************************************************
 * Main function
 ************************************************************/
int testcase1_3(void)
{
	//Initialize the ROSA kernel
	ROSA_Extended_Init();

	//Create semaphores
	Semaphore1 = 1;
	Task1_Array[0] = Semaphore1;
	Task2_Array[0] = Semaphore1;

	ROSA_semaphoreCreate(&Semaphore1);
	//Create tasks and install them into the ROSA kernel
	tcbHandle t1task1;
	ROSA_tcbCreate(&t1task1, "tsk1", task1_1, t1_stack, 0x040, T1_PRIORITY,NULL,Task1_Array,1);
 	
	//Start the ROSA kernel
	ROSA_Extended_Start();
	/* Execution will never return here */
	while(1);
}
