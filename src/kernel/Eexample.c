
/*
 * Eexample.c
 *
 * Created: 12/13/2016 5:23:43 PM
 *  Author: sam
 */ 
#include "kernel/rosa_ker.h"
#include "kernel/rosa_scheduler.h"
#include "kernel/rosa_sem.h"

#include "drivers/led.h"
#include <avr32/io.h>
#include "rosa_config.h"

#define MY_STACK_SIZE 0x100
static int t1_stack[MY_STACK_SIZE];
static int t2_stack[MY_STACK_SIZE];


void task1ex(void* tcbArg) {
	ticktime ticks = ROSA_getTicks();

	while (1) {
		// Toggle LED0
		ledToggle(LED0_GPIO);

		// Sleep for 1000 ms
		ROSA_taskDelayUntil(&ticks,1000);
	}
}

void task2ex(void* tcbArg) {
	ticktime ticks;
	ROSA_taskDelay(500);// offset blinking 500ms
	ticks = ROSA_getTicks();
	
	while (1) {
		// Toggle LED1
		ledToggle(LED1_GPIO);

		// Sleep for 1000 ms
		ROSA_taskDelayUntil(&ticks,1000);
	}
}

void example(void) {
	tcbHandle tHandle;
	
	//Initialization Extended ROSA
	ROSA_Extended_Init();
	
	//Create tcb task without using semaphores
	ROSA_tcbCreate(&tHandle,"task1",task1ex,t1_stack,MY_STACK_SIZE,1,NULL,NULL,0);
	ROSA_tcbCreate(&tHandle,"task2",task2ex,t2_stack,MY_STACK_SIZE,1,NULL,NULL,0);
	
	//Start Extended ROSA Scheduler
	ROSA_Extended_Start();
	
	// If it gets here, something is wrong...
	while(1);
}