/*
* testCase1.c
*
* Created: 2016-12-12 19:52:58
*  Author: Marcus Ventovaara

Description:
task_4 which blinks LED1 with an interval of 1s is 
continuously created and deleted by task_1 and task_2 respectively.
Tests the systems ability to re-use variables and the integrity of 
the create and delete functions.

task_3 is a control task to see if the system is still running.
*/
#include <avr32/io.h>

//Kernel includes
#include "kernel/rosa_ker.h"
//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"
#include "kernel/rosa_scheduler.h"

//Include configuration
#include "rosa_config.h"
#include "kernel/rosa_tim.h"


//Extra dependencies
//#include "semaphore.h"

#define STACK_SIZE 0x100
static int stack1[STACK_SIZE];
static int stack2[STACK_SIZE];
static int stack3[STACK_SIZE];
static int stack4[STACK_SIZE];


tcbHandle t1, t2, t3, t4;
semHandle s1;


int task_4(void)
{
	while(1)
	{
		ledToggle(LED1_GPIO);
		ROSA_taskDelay(1000);
	}
	return 0;
}

int counter = 0;
int task_1(void)
{
	ticktime t;
	while(1)
	{
		t = ROSA_getTicks();
		if(ROSA_semaphoreTake(s1) == 0){
			ledOn(LED4_GPIO);
			if(counter <= 1){
				ROSA_tcbCreate(&t4, "tsk4", task_4, stack4, STACK_SIZE, 5, NULL, NULL, 0);
				counter++;
			}
			ledOff(LED4_GPIO);
			ROSA_semaphoreGive(s1);
			ROSA_taskDelayUntil(&t, 200);
		}
	}
	return 0;
}

int task_2(void)
{
	ticktime t;
	while(1)
	{
		t = ROSA_getTicks();
		if(ROSA_semaphoreTake(s1) == 0){
			ledOn(LED6_GPIO);
			if(counter > 0){
				counter--;
				ROSA_tcbDelete(t4);
			}
			ledOff(LED6_GPIO);
			ROSA_semaphoreGive(s1);
			ROSA_taskDelayUntil(&t, 200);
		}
	}
	return 0;
}

int task_3(void)
{
	while(1)
	{
		ledOn(LED0_GPIO);
		ROSA_taskDelay(500);
		ledOff(LED0_GPIO);
		
		ROSA_taskDelay(500);
	}
}

int testcase1_m(void)
{
	// Check if ROSA is initiated
	if(ROSA_Extended_Init() != 0){
		return 0;
	}
	ROSA_semaphoreCreate(&s1);

	ROSA_tcbCreate(&t1, "tsk1", task_1, stack1, STACK_SIZE, 2, NULL, &s1, 1);
	ROSA_tcbCreate(&t2, "tsk2", task_2, stack2, STACK_SIZE, 2, NULL, &s1, 1);
	ROSA_tcbCreate(&t3, "tsk3", task_3, stack3, STACK_SIZE, 4, NULL, NULL, 0);
	
	
	// Start ROSA
	if (ROSA_Extended_Start() != 0){
		return 0;
	}
	while(1);

	return 0;
}
