/*
 * semtest2.c
 *
 * Created: 12/12/2016 9:21:31 AM
 *  Author: sam
 */ 

#include "kernel/rosa_ker.h"
#include "kernel/rosa_scheduler.h"
#include "kernel/rosa_sem.h"

#include "drivers/led.h"
#include <avr32/io.h>
#include "rosa_config.h"

semHandle sem_2[3] = {0};

#define MY_STACK_SIZE 0x100
static int t1_stack[MY_STACK_SIZE];
static int t2_stack[MY_STACK_SIZE];
//static const char *texttest = "Task1 is running\n";

void task1_2(void* tcbArg) {
	//char * taskname;
	//taskname=(char*)tcbArg;
	//usartWriteLine(USART, taskname);
	ticktime ticks = ROSA_getTicks();
	int i;
	int tmp;

	while (1) {
		// Toggle LED1
		for (i=0; i<300000; i++) {
			//ledOff(LED0_GPIO);
			ledOn(LED0_GPIO);
			if (i == 100000) {
				while(ROSA_semaphoreTake(sem_2[1]));
				ledOn(LED4_GPIO);
				ROSA_taskDelay(4000);
				ledOff(LED3_GPIO);
				} else if (i == 200000) {
				ledOff(LED4_GPIO);
				ROSA_semaphoreGive(sem_2[1]);
			}
			//usartWriteChar(USART, 'A');
		}
		
		ledOff(LED0_GPIO);

		// Sleep for 1000 ms
		//ROSA_taskDelayUntil(&ticks,5000);
		ROSA_taskDelay(1000);
	}
}

void task2_2(void* tcbArg) {
	ticktime ticks;
	int i;

	ROSA_taskDelay(1500);
	ticks =  ROSA_getTicks();

	while (1) {
		// Toggle LED2
		
		for (i=0; i<300000; i++) {
			//ledOff(LED1_GPIO);
			ledOn(LED1_GPIO);
			if (i == 100000) {
				while(1){
					int error = ROSA_semaphoreTake(sem_2[1]);
					if(error != 0){
						ledOn(LED3_GPIO);
						usartWriteChar(USART, '-');
						queue_display(READYQUEUE);
						usartWriteChar(USART, '+');
					}
					else{
						ledOff(LED3_GPIO);
						break;
					}
				}
				ledOn(LED6_GPIO);
				} else if (i == 200000) {
				ledOff(LED6_GPIO);
				ROSA_semaphoreGive(sem_2[1]);
			}
		}

		ledOff(LED1_GPIO);

		// Sleep for 1000 ms
		//ROSA_taskDelayUntil(&ticks,5000);
		ROSA_taskDelay(5000);
	}
}

void semaphoreTest2() {
	tcbHandle t1Handle;
	tcbHandle t2Handle;
	
	ROSA_Extended_Init();
	
	ROSA_semaphoreCreate(&sem_2[0]);
	ROSA_semaphoreCreate(&sem_2[1]);
	ROSA_semaphoreCreate(&sem_2[2]);
	
	ROSA_tcbCreate(&t1Handle,"tsk1",task1_2,t1_stack,MY_STACK_SIZE,2,NULL,sem_2,3);
	ROSA_tcbCreate(&t2Handle,"tsk2",task2_2,t2_stack,MY_STACK_SIZE,3,NULL,&sem_2[1],2); 
	//Deletes task 1
	//ROSA_tcbDelete(&t2Handle);
	
	ROSA_Extended_Start();
}