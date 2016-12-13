/*
* delayUntil_test.c
*
* Created: 2016-12-13 20:41:48
*/

//Standard library includes
#include <avr32/io.h>

//Kernel includes
#include "kernel/rosa_ker.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"
//#include "kernel/rosa_sem.h"
//#include "kernel/rosa_prv.h"
#include "test.h"
//#include "rosa_idle.h"


//Include configuration
#include "rosa_config.h"

//Data blocks for the tasks
//#define T1_STACK_SIZE 0x40
#define T1_STACK_SIZE 0x100
static int t1_stack[T1_STACK_SIZE];
static tcb t1_tcb;





void taskToDelay(void)
{
	ticktime start = ROSA_getTicks();

	while(1) {

		int count = 0;
		while (count < 10000){
			count++;
		}
		ledOn(LED2_GPIO);
		int o = ROSA_taskDelayUntil(&start, 1);
		usartWriteChar(USART, o + '0');

		ledOff(LED2_GPIO);
		ROSA_taskDelayUntil(&start, 1000);
		
		if (o == 0){
			ledOn(LED4_GPIO);
			delay_ms(1000);
			ledOff(LED4_GPIO);
			delay_ms(1000);
			
		}
	}
}


void delayUnt_test(void)
{


	//Initialize the ROSA kernel
	ROSA_Extended_Init();

	void* arguments = NULL;
	semHandle* semaphore = NULL;
	int sem_number = 0;

	ROSA_tcbCreate(&t1_tcb,"prod", taskToDelay, t1_stack, T1_STACK_SIZE, 1, arguments, semaphore, sem_number);
	
	ROSA_Extended_Start();
	while(1);
}

