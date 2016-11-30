/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///' '///,'/////',/////'  /////'/;,

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

#ifndef _ROSA_KER_H_
#define _ROSA_KER_H_

#include "rosa_def.h"
#include "rosa_queue.h"

/***********************************************************
 * Global variables
 ***********************************************************/
extern tcb * TCBLIST;
extern tcb * EXECTASK;

queue* READYQUEUE;
queue* WAITINGQUEUE;


/***********************************************************
 * Kernel low level context switch functions
 ***********************************************************/
extern void ROSA_contextInit(tcb * tcbTask);
extern void ROSA_yield(void);


/***********************************************************
 * Other kernel functions
 ***********************************************************/
//None

/***********************************************************
 * General kernel API
 *
 * Comment:
 *	No comments available.
 *
 **********************************************************/

//Define the initial value of the satus register
#define ROSA_INITIALSR 0x1c0000

//Timer interrupt service routine
__attribute__((__interrupt__)) extern void timerISR(void);

//Initialize the kernel
void ROSA_init(void);
void ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize);

int ROSA_Extended_Init(void);
int ROSA_Extended_Start(void);

//Install a new task TCB into ROSA
extern void ROSA_tcbInstall(tcb *task);

//Start running the ROSA kernel
//This start running the created and installed tasks.
extern void ROSA_start(void);

tcb * ROSA_prvGetFirstFromReadyQueue(void);
int ROSA_prvAddToReadyQueue(tcb *task);
int ROSA_prvRemoveFromReadyQueue(tcb *task);
int ROSA_prvUpdateReadyQueue(tcb *modifiedTask);

tcb * ROSA_prvGetFirstFromWaitingQueue(void);
//wake_time should be of the proper type
int ROSA_prvAddToWaitingQueue(tcb *task, unsigned int wake_time);
int ROSA_prvRemoveFromWaitingQueue(tcb *task);
int ROSA_prvDecreasetWaitingQueueValues(unsigned int offset);

#endif /* _ROSA_KER_H_ */
