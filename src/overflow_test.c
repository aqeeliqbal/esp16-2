/*
 * overflow_test.c
 *
 * Created: 2017-01-06 22:24:09
 */ 
#include "kernel/rosa_ker.h"
#include "kernel/rosa_scheduler.h"
#include "kernel/rosa_sem.h"

#include "drivers/led.h"
#include <avr32/io.h>
#include "rosa_config.h"
#include "kernel/rosa_queue.h"

#define MY_STACK_SIZE 0x100
static int t1_stack[MY_STACK_SIZE];
static int t2_stack[MY_STACK_SIZE];
static int t3_stack[MY_STACK_SIZE];

void t_1(void) {

	ticktime ticks = ROSA_getTicks();

	int i;
	while (1) {
		ledOn(LED0_GPIO);
		int count=0;
		ROSA_taskDelayUntil(&ticks,3000);
		ledOff(LED0_GPIO);
		ROSA_taskDelayUntil(&ticks,3000);
		

	}
}

void t_2(void) {
	

	ticktime ticks = ROSA_getTicks();

	while (1) {
		ledOn(LED1_GPIO);

ROSA_taskDelayUntil(&ticks,2000);
ledOff(LED1_GPIO);
ROSA_taskDelayUntil(&ticks,2000);
		


	}
}


void t_3(void) {
	

	ticktime ticks = ROSA_getTicks();

	while (1) {
		ledOn(LED3_GPIO);

		ROSA_taskDelay(1000);
		ledOff(LED3_GPIO);
		ROSA_taskDelay(1000);
		


	}
}


void OF_test() {
	tcbHandle t1Handle;
	tcbHandle t2Handle;
	tcbHandle t3Handle;
	
	ROSA_Extended_Init();
	
	

	
	ROSA_tcbCreate(&t1Handle,"tsk1",t_1,t1_stack,MY_STACK_SIZE,4,NULL,NULL,0);
	ROSA_tcbCreate(&t2Handle,"tsk2",t_2,t2_stack,MY_STACK_SIZE,3,NULL,NULL,0);
	ROSA_tcbCreate(&t3Handle,"tsk3",t_3,t3_stack,MY_STACK_SIZE,2,NULL,NULL,0);
	//Deletes task 1
	//ROSA_tcbDelete(&t2Handle);
	
	ROSA_Extended_Start();
}
