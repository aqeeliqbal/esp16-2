/*****************************************************************************

                 ///////,   .////    .///' /////,
                ///' ./// ///'///  ///,     '///
               ///////'  ///,///   '/// //;';//,
             ,///' ////,'/////',/////'  /////'/;,

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

//Kernel includes
#include "kernel/rosa_def.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"
#include "kernel/rosa_scheduler.h"

//Driver includes
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"
#include "drivers/usart.h"

/***********************************************************
 * TCBLIST
 *
 * Comment:
 * 	Global variables that contain the list of TCB's that
 * 	have been installed into the kernel with ROSA_tcbInstall()
 **********************************************************/
tcb * TCBLIST;

/***********************************************************
 * EXECTASK
 *
 * Comment:
 * 	Global variables that contain the current running TCB.
 **********************************************************/
tcb * EXECTASK;

/***********************************************************
 * ROSA_init
 *
 * Comment:
 * 	Initialize the ROSA system
 *
 **********************************************************/
void ROSA_init(void)
{
	//Do initialization of I/O drivers
	ledInit();									//LEDs
	buttonInit();								//Buttons
	joystickInit();								//Joystick
	potInit();									//Potentiometer
	usartInit(USART, &usart_options, FOSC0);	//Serial communication

	//Start with empty TCBLIST and no EXECTASK.
	TCBLIST = NULL;
	EXECTASK = NULL;

	//Initialize the timer to 1 ms period.
	//...
	
	//...
}

/***********************************************************
 * ROSA_tcbCreate
 *
 * Comment:
 * 	Create & Install the TCB with correct values.
 *
 **********************************************************/
void ROSA_tcbCreate(tcbHandle *tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize, int taskPriority, void *tcbArg, semHandle  *semaphores, int semaCount)
{
	int i;

	//Initialize the tcb with the correct values
	for(i = 0; i < NAMESIZE; i++) {
		//Copy the id/name
		tcbTask->id[i] = tcbName[i];
	}

	//Dont link this TCB anywhere yet.
	tcbTask->nexttcb = NULL;

	//Set the task function start and return address.
	tcbTask->staddr = tcbFunction;
	tcbTask->retaddr = (int)tcbFunction;

	//Set up the stack.
	tcbTask->datasize = tcbStackSize;
	tcbTask->dataarea = tcbStack + tcbStackSize;
	tcbTask->saveusp = tcbTask->dataarea;

	//Set the initial SR.
	tcbTask->savesr = ROSA_INITIALSR;

	//Setting our custom values
	tcbTask->originalpriority = taskPriority;
	tcbTask->priority = taskPriority;

	for(int i =0; i<semaCount: i++)
	{
		ROSA_SemaphoreRegister(semaphores[i], tcbTask);
	}

	tcbTask->activeSemaphores = calloc(semaCount,sizeof(semHandle));
	
	tcb * tcbTmp;

	/* Is this the first tcb installed? */
	if(TCBLIST == NULL) {
		TCBLIST = tcbTask;
		TCBLIST->nexttcb = tcbTask;			//Install the first tcb
		tcbTask->nexttcb = TCBLIST;			//Make the list circular
	}
	else {
		tcbTmp = TCBLIST;					//Find last tcb in the list
		while(tcbTmp->nexttcb != TCBLIST) {
			tcbTmp = tcbTmp->nexttcb;
		}
		tcbTmp->nexttcb = tcbTask;			//Install tcb last in the list
		tcbTask->nexttcb = TCBLIST;			//Make the list circular
	}
	
	//Initialize context.
	contextInit(tcbTask);
}


/***********************************************************
 * ROSA_tcbInstall
 *
 * Comment:
 * 	Delete the TCB into the TCBLIST.
 *
 **********************************************************/
void ROSA_tcbDelete(tcbHandle *tcbTask, semHandle  *semaphores, int semaCount) 
{
	//To be provided by Ali and Per
	int result = 0;
	for(int i =0; i<semaCount: i++)
	{
		
		ROSA_SemaphoreUnregister(semaphores[i], tcbTask);
		{
		
	tcbTask->activeSemaphores = calloc(semaCount,sizeof(semHandle)); 
	}
	if(result == 0)

	ROSA_prvRemoveFromReadyQueue(tcbTask);
	ROSA_prvRemoveFromWaitingQueue(tcbTask);

	tcb * tcbTmp;
	tcb * tcbNextTask;

		tcbTmp = TCBLIST;					
		while(tcbTmp->nexttcb != tcbTask) {
			tcbTmp = tcbTmp->nexttcb;
		}
		tcbTmp->nexttcb = tcbTask->nexttcb;			
		tcbTask->nexttcb = NULL;	
		tcbTask = NULL;		
	
}
/***********************************************************
 *  ROSA_tcbResume
 *
 * Comment:
 * 	
 *
 **********************************************************/
void ROSA_tcbResume(void *tcbTask)
{
	checkinTCBLIST(tcb *task);
	if(error == 0) //assuming that there is no error, it returns 0
	{
	ROSA_tcbSuspend(void *tcbTask)
	}
	//now return error variable
	if(error == 0) //meaning task has already been suspended.
	{
	return ROSA_prvAddToReadyQueue(tcbTask);
	}
/***********************************************************
 * ROSA_tcbSuspend
 *
 * Comment:
 * 	Delete the TCB into the TCBLIST.
 *
 **********************************************************/
void ROSA_tcbSuspend(void *tcbTask)
{
     int errorMessage;
	 if( (ROSA_prvCheckInReadyQueue(tcb *task)==0) && (ROSA_prvCheckInWaitingQueue(tcb *task)==0) ) // check if it is not in Ready and waiting Q
	 if(errorMessage){
		 return errorMessage +3 ;
		 }
	 if ((checkinTCBLIST(tcb *task)==0)) // check if this task exists
	 if(errorMessage){
		 return errorMessage +4 ;
	 }
	//Now check what ROSA_prvRemoveFromWaitingQueue(tcbTask) returns and just put that value.
	error = ROSA_prvRemoveFromReadyQueue(tcbTask);
	if(error == 0) //assuming that there is no error, it returns 0
	{
		int return_value  = ROSA_prvAddToWaitingQueue(tcbTask);
	}
	//now return error variable
	return  0 ;
}	
/***********************************************************
 * ROSA_tcbSuspend
 *
 * Comment:
 * 	Delete the TCB into the TCBLIST.
 *
 **********************************************************/
checkinTCBLIST(tcbTask);
for(i = 0; i < TCBLIST; i++)
{
	if ((TCBLIST[i].taskID == tcbTask.id))
	{
	RETURN 0;
	}
	//if found
	break;
}
	
//0 everything is OK
//1 READYQUEUE failed to initialize
//2 WAITINGQUEUE failed to initialize
int ROSA_Extended_Init(void){
	
	if(queue_init(READYQUEUE)) return 1;
	if(queue_init(WAITINGQUEUE)) return 2;
	
	timerInit(100);
	timerPeriodSet(1);
	interruptInit();
	interruptEnable();
	ROSA_init();
	//create idle task
	return 0;
}
int ROSA_Extended_Start(void){
	timerStart();
	
	ROSA_start();
	return 0;
}

tcb * ROSA_prvGetFirstFromReadyQueue(void){
	tcb* firstTask = queue_getFirst(READYQUEUE)->task_tcb;
	//if firstTask == NULL, give the Idle task!
	return firstTask;
}

//0 1 and 2 are described in queue_push
//3 null pointer
//4 task already in the queue
int ROSA_prvAddToReadyQueue(tcb *task){
	if(task == NULL){
		return 3;
	}
	if(queue_getPosition(READYQUEUE, task) != -1){
		return 4;
	}
	queue_item new_item;
	new_item.task_tcb = task;
	new_item.value = task->priority;
	return queue_push(READYQUEUE, &new_item, 1);
}
	
	

//0 everything is ok
//1 null pointer
//2 task not in the queue
//3 removal failed
int ROSA_prvRemoveFromReadyQueue(tcb *task){
	if(task == NULL){
		return 1;
	}
	if(queue_getPosition(READYQUEUE, task) == -1){
		return 2;
	}
	if(queue_remove(READYQUEUE, task, 1).task_tcb == NULL){
		return 3;
	}
	return 0;
}


//0 everything is ok
//1 2 3 defined in ROSA_prvRemoveFromReadyQueue
//4 5 6 7 defined in ROSA_prvAddToReadyQueue
int ROSA_prvUpdateReadyQueue(tcb *modifiedTask){
	int errorMessage;
	errorMessage = ROSA_prvRemoveFromReadyQueue(modifiedTask);
	if(errorMessage){
		return errorMessage;
	}
	errorMessage = ROSA_prvAddToReadyQueue(modifiedTask);
	if(errorMessage){
		return errorMessage + 3;
	}
	return 0;
}

tcb * ROSA_prvGetFirstFromWaitingQueue(void){
	tcb* firstTask = queue_getFirst(WAITINGQUEUE)->task_tcb;
	//if firstTask == NULL, give the Idle task!
	return firstTask;
}

unsigned int ROSA_prvGetFirstWakeTime(void){
	return queue_getFirst(WAITINGQUEUE)->value;
}

//0 1 and 2 are described in queue_push
//3 null pointer
//4 task already in the queue
//wake_time should be of the proper type
int ROSA_prvAddToWaitingQueue(tcb *task, unsigned int wake_time){
	if(task == NULL){
		return 3;
	}
	if(queue_getPosition(WAITINGQUEUE, task) != -1){
		return 4;
	}
	queue_item new_item;
	new_item.task_tcb = task;
	new_item.value = wake_time;
	return queue_push(WAITINGQUEUE, &new_item, 0);
}

//0 everything is ok
//1 null pointer
//2 task not in the queue
//3 removal failed
int ROSA_prvRemoveFromWaitingQueue(tcb *task){
	if(task == NULL){
		return 1;
	}
	if(queue_getPosition(WAITINGQUEUE, task) == -1){
		return 2;
	}
	if(queue_remove(WAITINGQUEUE, task, 0).task_tcb == NULL){
		return 3;
	}
	return 0;
}

//return values defined in queue_decreaseValues
int ROSA_prvDecreasetWaitingQueueValues(unsigned int offset){
	return queue_decreaseValues(WAITINGQUEUE, offset);
}

//return values defined by ROSA_prvUpdateReadyQueue
int ROSA_prvRaiseTaskPriority(tcb *task, unsigned int new_priority){
	task->priority = new_priority;
	return ROSA_prvUpdateReadyQueue(task);
}

//return values defined by ROSA_prvUpdateReadyQueue
int ROSA_prvResetTaskPriority(tcb *task){
	task->priority = task->original_priority;
	return ROSA_prvUpdateReadyQueue(task);
}

// 0 - task is not in the ready queue
// 1 - task found in the ready queue
// 2 - null pointer, invalid input
int ROSA_prvCheckInReadyQueue(tcb *task){
	if(task == NULL){
		return 2;
	}
	int return_value = queue_getPosition(READYQUEUE, task);
	if(return_value == -1){
		return 0;
	}
	else{
		return 1;
	}
}

// 0 - task is not in the waiting queue
// 1 - task found in the waiting queue
// 2 - null pointer, invalid input
int ROSA_prvCheckInWaitingQueue(tcb *task){
	if(task == NULL){
		return 2;
	}
	int return_value = queue_getPosition(WAITINGQUEUE, task);
	if(return_value == -1){
		return 0;
	}
	else{
		return 1;
	}
}
