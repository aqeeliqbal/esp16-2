/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// ///''\\,
             ,///' '///,'/////',/////'  /////'\\,

    Copyright 2010 Marcus Jansson <mjansson256@yahoo.se>

    This file is part of ROSA - Realtime Operating System for AVR32.

    ROSA is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROSA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ROSA.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
/* Tab size: 4 */

//Standard library includes
#include <avr32/io.h>

//Kernel includes
#include "kernel/rosa_ker.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"
#include "kernel/rosa_sem.h"
#include "kernel/rosa_prv.h"


//Include configuration
#include "rosa_config.h"

//Data blocks for the tasks
//#define T1_STACK_SIZE 0x40
#define T1_STACK_SIZE 0x100
static int t1_stack[T1_STACK_SIZE];
//static tcb t1_tcb;

#define T2_STACK_SIZE 0x100
static int t2_stack[T2_STACK_SIZE];
//static tcb t2_tcb;
tcbHandle t1_tcb;
tcbHandle t2_tcb;



/**************************/
/*
int ROSA_taskDelayUntil(ticktime start, ticktime t){
	tcb *readyP = NULL;
	ticktime maxClock = 4294967295;
		ticktime sum = start + t;
		ticktime rest = maxClock - start;
		int err;
	/*	if (t > 3900000000){
			return 1;
		}
		else if (t > rest) && ( t< 4294967295){
			ROSA_clockOverflow();
		}
		else if (t < rest) && ((start + t) < now=ROSA_getTicks()){
			ROSA_prvAddToReadyQueue(&EXECTASK);
		}
		else{*/
		//interruptDisable();
		//void contextSave();
		/*readyP = ROSA_prvGetFirstFromReadyQueue();
		err = ROSA_prvRemoveFromReadyQueue(readyP);
		
		usartWriteChar(USART, err + '0');
		ROSA_prvAddToWaitingQueue(readyP, sum);
		//void contextRestore();
		//interruptEnable();
		ROSA_yield();
		return 0;
		//}
}
*/

/*************************/


/*************************************************************
 * Task1
 * LED0 lights up
 * LED1 goes dark
 ************************************************************/
void task1(void)
{
	//int t;
	while(1) {
		//usartWriteTcb(USART, &t1_tcb);
		
		
		
		ticktime start = ROSA_getTicks();
		while(ROSA_getTicks() < start + 1000);
		ledOn(LED0_GPIO);
		start = ROSA_getTicks();
		while(ROSA_getTicks() < start + 1000);
		ledOff(LED0_GPIO);
		//ROSA_prvRemoveFromReadyQueue(&t1_tcb);
		//ROSA_prvAddToWaitingQueue(&t1_tcb, ROSA_getTicks() + 1000);
		ROSA_taskDelayUntil(start, 5000);
		ROSA_yield();
	}
}

/*************************************************************
 * Task2 hello it Ali
 * LED0 goes dark
 * LED1 lights up 
 ************************************************************/
void task2(void)
{

	//int t;
	while(1) {
		//usartWriteTcb(USART, &t2_tcb);
		ticktime start = ROSA_getTicks();
		while(ROSA_getTicks() < start + 1000);
		ledOn(LED1_GPIO);
		start = ROSA_getTicks();
		while(ROSA_getTicks() < start + 1000);
		ledOff(LED1_GPIO);
		
		//ROSA_prvRemoveFromReadyQueue(&t2_tcb);
		//ROSA_prvAddToWaitingQueue(&t2_tcb, ROSA_getTicks() + 1000);
		
		//usartWriteChar(USART, 'R');
		//queue_display(READYQUEUE);
		ROSA_taskDelayUntil(start, 5000);
		ROSA_yield();
	}
}



/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_Extended_Init();
	
	
	

	//Create tasks and install them into the ROSA kernel
	void* args;
	semHandle* semaphores;
	int sem_number = 3;
	
	//Creates two tasks
	ROSA_tcbCreate(&t1_tcb, "tsk1", task1, t1_stack, T1_STACK_SIZE, 3, args, semaphores, sem_number);
	ROSA_tcbCreate(&t2_tcb, "tsk2", task2, t2_stack, T2_STACK_SIZE, 2, args, semaphores, sem_number);
	//should print out the two tasks and the idle task
	usartWriteChar(USART, 'M');
	queue_display(READYQUEUE);
	//Deletes task 1
	ROSA_tcbDelete(&t1_tcb);
	//Suspends task 2
	ROSA_tcbSuspend(&t2_tcb);
	//Resumes task 2
	ROSA_tcbResume(&t2_tcb);
	//Cretes task 1 again
	ROSA_tcbCreate(&t1_tcb, "tsk1", task1, t1_stack, T1_STACK_SIZE, 3, args, semaphores, sem_number);

	//Start the ROSA kernel
	ROSA_Extended_Start();
	/* Execution will never return here */
	while(1){
	}
}
