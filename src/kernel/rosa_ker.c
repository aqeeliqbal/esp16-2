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
//SABA
#include "kernel/rosa_sem.h"
#include "kernel/rosa_prv.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_scheduler.h"

//Driver includes
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"
#include "drivers/usart.h"

#include "stdlib.h"



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
}

// 0 - everything is ok
// 1 - ROSA not initiated
// 2 - MAX_TASK_NUMBER reached
// 3 - invalid task priority
// 4 - idle task already created
int ROSA_tcbCreate(tcbHandle *tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize, unsigned int taskPriority, void *tcbArg, semHandle  *semaphores, int semaCount)
{
	//ERROR CHECKS
	if(rosa_initiated != 1){
		return 1;
	}
	if(task_number >= MAX_TASK_NUMBER){
		return 2;
	}
	if(taskPriority < IDLE_TASK_PRIORITY || taskPriority > MAX_TASK_PRIORITY){
		return 3;
	}
	
	//Make sure only one idle task is created
	if(idle_task_created != 1 && taskPriority == IDLE_TASK_PRIORITY){
		idle_task_created = 1;
	}
	else if(idle_task_created == 1 && taskPriority == IDLE_TASK_PRIORITY){
		return 4;
	}
	
	int i;
	
	tcb * task = (tcb *) malloc(sizeof(tcb));

	//Initialize the tcb with the correct values
	for(i = 0; i < NAMESIZE; i++) {
		//Copy the id/name
		task->id[i] = tcbName[i];
	}

	//Dont link this TCB anywhere yet.
	task->nexttcb = NULL;

	//Set the task function start and return address.
	task->staddr = tcbFunction;
	task->retaddr = (int)tcbFunction;

	//Set up the stack.
	task->datasize = tcbStackSize;
	task->dataarea = tcbStack + tcbStackSize;
	task->saveusp = task->dataarea;

	//Set the initial SR.
	task->savesr = ROSA_INITIALSR;

	//Setting our custom values
	task->original_priority = taskPriority;
	task->priority = taskPriority;
	task->tcbArg = tcbArg;	

	task->semaList = calloc(semaCount,sizeof(semHandle));
	task->semaCount = semaCount;
	
	for (i=0; i<semaCount && semaphores != NULL; i++) {
		task->semaList[i] = semaphores[i];
		ROSA_prvSemaphoreRegister(semaphores[i],task);
	}
	
	int result = 0;
	
	tcb * tcbTmp;

	/* Is this the first tcb installed? */
	if(TCBLIST == NULL) {
		TCBLIST = task;
		TCBLIST->nexttcb = task;			//Install the first tcb
		task->nexttcb = TCBLIST;			//Make the list circular
	}
	else {
		tcbTmp = TCBLIST;					//Find last tcb in the list
		while(tcbTmp->nexttcb != TCBLIST) {
			tcbTmp = tcbTmp->nexttcb;
		}
		tcbTmp->nexttcb = task;			//Install tcb last in the list
		task->nexttcb = TCBLIST;			//Make the list circular
	}
	
	*tcbTask = task;
	
	ROSA_prvAddToReadyQueue(task);
	task_number++;
	
	//Initialize context.
	contextInit(task);
	return 0;
}

// 0 - everything is ok
// 1 - the task isn't created
// 2 - semaphore unregistration field 
// 3 - removing from ready queue failed
// 4 - removing from waiting queue failed
int ROSA_tcbDelete(tcbHandle *task)
{
	int errorMessage,i;
	tcb *tcbTask = (tcb*)*task;
	if (ROSA_prvcheckinList(TCBLIST, tcbTask) != 1)
	{
		return 1;
	}
	for ( i=0; i<tcbTask->semaCount; i++) {
		int error = ROSA_prvSemaphoreUnregister(tcbTask->semaList[i],tcbTask);
		if (error != 0)
		{
			return 2;
		}
		
	}
	errorMessage = ROSA_prvRemoveFromReadyQueue(tcbTask);
	if(errorMessage != 0 && errorMessage != 2){
		return 3;
	}
	errorMessage = ROSA_prvRemoveFromWaitingQueue(tcbTask);
	if(errorMessage != 0 && errorMessage != 2){
		return 4;
	}
	
	
	tcb * tcbTmp;

	tcbTmp = TCBLIST;
	while(tcbTmp->nexttcb != tcbTask) {
		tcbTmp = tcbTmp->nexttcb;
	}
	tcbTmp->nexttcb = tcbTask->nexttcb;
	tcbTask->nexttcb = NULL;
	
	free(tcbTask);
	task = NULL;
	task_number--;
	return 0;
}

//2 - the list is empty
//1 - task is in the list
//0 - task is not in the list
int ROSA_prvcheckinList(tcb *list, tcb *task)
{
	tcb * tcbTmp;
	if(list == NULL){
		return 2;
	}
	tcbTmp = list;					//Find last tcb in the list

	do {
		if(tcbTmp == task){
			task = tcbTmp;
			return 1;
		}
		tcbTmp = tcbTmp->nexttcb;		
	} while(tcbTmp != list);

	return 0;
}

// 0 - everything is ok
// 1 - task is null pointer
// 2 - task isn't created
// 3 - task is already suspended
// 4 - removing from ready queue failed
// 5 - removing from waiting queue failed
int ROSA_tcbSuspend(tcbHandle *task)
{
	int errorMessage;
	if(task == NULL){
		return 1;
	}
	tcb *tcbTask = (tcb*)*task;
	
	if(ROSA_prvcheckinList(TCBLIST,tcbTask) != 1){
		//it isn't created
		return 2;
	}
	
	if(ROSA_prvCheckInReadyQueue(tcbTask) == 0 && ROSA_prvCheckInWaitingQueue(tcbTask) == 0){
		//it is already suspended
		return 3;
	}
	
	errorMessage = ROSA_prvRemoveFromReadyQueue(tcbTask);
	if(errorMessage != 0 && errorMessage != 2){
		return 4;
	}
	
	errorMessage = ROSA_prvRemoveFromWaitingQueue(tcbTask);
	if(errorMessage != 0 && errorMessage != 2){
		return 5;
	}
	return 0;
}

// SABA
// 0 - everything is ok
// 1 - task is null pointer
// 2 - task isn't created
// 3 - task is not suspended
// 4 - adding from ready queue failed
int ROSA_tcbResume(tcbHandle *task)
{
	int errorMessage;
	if(task == NULL){
		return 1;
	}
	tcb *tcbTask = (tcb*)*task;
	
	if(ROSA_prvcheckinList(TCBLIST,tcbTask) != 1){
		//it isn't created
		return 2;
	}
	
	if(ROSA_prvCheckInReadyQueue(tcbTask) == 1 || ROSA_prvCheckInWaitingQueue(tcbTask) == 1){
		//it is not suspended
		return 3;
	}
	
	errorMessage = ROSA_prvAddToReadyQueue(tcbTask);
	if(errorMessage != 0 && errorMessage != 2){
		return 4;
	}
	return 0;
}



//0 everything is OK
//1 READYQUEUE failed to initialize
//2 WAITINGQUEUE failed to initialize
//3 ROSA already initiated
int ROSA_Extended_Init(void){
	//ERROR CHECK
	if(rosa_initiated == 1){
		return 3;
	}
	rosa_initiated = 1;
	idle_task_created = 0;
	task_number = 0;
	READYQUEUE = (queue * ) malloc(sizeof(queue));
	WAITINGQUEUE = (queue * ) malloc(sizeof(queue));
	
	if(queue_init(READYQUEUE)) return 1;
	if(queue_init(WAITINGQUEUE)) return 2;
	
	interruptInit();
	timerInit(1);
	timerPeriodSet(1);
	//interruptInit();
	//interruptEnable();
	
	ROSA_init();
	
	void* idle_args = NULL;
	semHandle* semaphores = NULL;
	int sem_number = 0;
	
	return ROSA_tcbCreate(&idle_tcb, "idle", idle, idle_stack, IDLE_STACK_SIZE, IDLE_TASK_PRIORITY, idle_args, semaphores, sem_number) + 3;

}

// 0 - everything is ok
// 1 - ROSA not initiated
// 2 - ROSA already started
int ROSA_Extended_Start(void){
	//ERROR CHECKS
	if(rosa_initiated != 1){
		return 1;
	}
	if(rosa_started == 1){
		return 2;
	}
	rosa_started = 1;
	
	timerStart();
	
	ROSA_start();
	return 0;
}

tcb * ROSA_prvGetFirstFromReadyQueue(void){
	queue_item *item = queue_getFirst(READYQUEUE);
	if(item == NULL){
		return NULL;
	}
	tcb* firstTask = item->task_tcb;	
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
	queue_item *item = queue_getFirst(WAITINGQUEUE);
	if(item == NULL){
		return NULL;
	}
	tcb* firstTask = item->task_tcb;
		
	return firstTask;
}

unsigned int ROSA_prvGetFirstWakeTime(void){
	if( WAITINGQUEUE->count == 0){
		return MAX_TICK_COUNT;
	}
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
