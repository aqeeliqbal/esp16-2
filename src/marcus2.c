/*****************************************************************************
We have three tasks and two semaphores (s1 and s2).
Task1 and Task2 want to have s1. They should take and give the semaphore to each other.
If task1 alone or task2 alone blink all the time, then they fail to pass the semaphores to each other.

*****************************************************************************/
// *  Author: Group 4

//Standard library includes
#include <avr32/io.h>

//Kernel includes
#include "kernel/rosa_ker.h"
//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"
#include "kernel/rosa_scheduler.h"

//#include "semaphore.h"

//Include configuration
#include "rosa_config.h"
#include "kernel/rosa_tim.h"

#define IDLE_PRIO 0

//Data blocks for the tasks
#define T1_STACK_SIZE 0x80
static int t12_m_stack[T1_STACK_SIZE];

#define T2_STACK_SIZE 0x80
static int t22_m_stack[T2_STACK_SIZE];

#define T3_STACK_SIZE 0x80
static int t32_m_stack[T2_STACK_SIZE];


//task handles
tcbHandle t12_m, t22_m, t32_m;

semHandle s12_m, s22_m;

/*************************************************************
* Task1
* LED0 toggle.
* use semaphore s1.
************************************************************/
void task12_m(void)
{
	int cc = 0;
	while(1) {
		ROSA_semaphoreTake(s12_m);
		while (cc < 10){
			ledToggle(LED0_GPIO);
			delay_ms(500);
			cc++;
		}
		cc = 0;
		ROSA_semaphoreGive(s12_m);
		ROSA_taskDelay(50);

	}
}

/*************************************************************
* Task2
* LED1 toggle.
* use semaphore s1.
************************************************************/
void task22_m(void)
{
	int cc = 0;
	while(1) {
		ROSA_semaphoreTake(s12_m);
		while (cc < 10){
		ledToggle(LED1_GPIO);
		delay_ms(250);
		cc++;
		}
		cc = 0;
		ROSA_semaphoreGive(s12_m);
		ROSA_taskDelay(50);

	}
}
// Takes semaphore s2. Toggle Led2
void task32_m(void)
{
	int cc = 0;

	while(1) {
		ROSA_semaphoreTake(s22_m);
		while (cc < 10){
		ledToggle(LED2_GPIO);
		delay_ms(200);
		cc++;
		}
		cc = 0;
		ROSA_semaphoreGive(s22_m);
	}
}

/*************************************************************
* Main function
************************************************************/
int main_m2(void)
{
	// Check if ROSA is initiated 
	if(ROSA_Extended_Init() != 0){
		return 0;
	}
	// Create semaphores
	ROSA_semaphoreCreate(&s12_m);
	ROSA_semaphoreCreate(&s22_m);

	
	ROSA_tcbCreate(&t12_m, "tsk1", task12_m, t12_m_stack, T1_STACK_SIZE, IDLE_PRIO+4, NULL, &s12_m, 1);
	ROSA_tcbCreate(&t22_m, "tsk2", task22_m, t22_m_stack, T2_STACK_SIZE, IDLE_PRIO+3, NULL, &s12_m, 1);
	ROSA_tcbCreate(&t32_m, "tsk3", task32_m, t32_m_stack, T3_STACK_SIZE, IDLE_PRIO+1, NULL, &s22_m, 1);
	
	// Start ROSA	
	if (ROSA_Extended_Start() != 0){
		return 0;
	}
	/* Execution will never return here */
	while(1);

	return 0;
}
