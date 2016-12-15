

#include "kernel/rosa_sem.h"
#include "kernel/rosa_prv.h"
#include "kernel/rosa_ker.h"

#include <stdlib.h>
/*#include <stdio.h>*/

/* Constants */

#define SEMAPHORE_ACTIVE 0x01
#define SEMAPHORE_LOCKED 0x02

/* Structs */

/*
  The struct for each post in the registration list

  prio - the priority of the task
  task - the task registered to this semaphore
  next - next post in list
*/

typedef struct sem_reglist_record_t {
    tcb* task;
    struct sem_reglist_record_t* next;
} semaphore_reglist;

/*
  This struct describes each semaphore
  flags   - Things like, is it deleted? single bit structure 000010010
  owner   - If NULL, the semaphore mutex is unlocked. Else, it is
            locked by this task.
  reglist - The registration list of the semaphore. The ceiling is
            the prio field of the first element in this list.
*/

typedef struct sem_record_t {
    uint8_t flags;
    tcb* owner;
    semaphore_reglist* reglist;
} semaphore;

/* Global variables not accessible outside this file */

semaphore* semaphores[16];

/* Internal functions used by this file only */
semaphore* ROSA_prvSemaphoreGet(semHandle sem);

/* Public function definitions */

unsigned int ROSA_semaphoreCreate(semHandle *sem) {
    uint16_t i = 0;                   /* sem will receive a value between 1 and 256 */

    *sem = 0;
    
    /* Search through the list for the first available spot in the list */

    for (i=0; i<256; i++) {
        if (semaphores[i/16] == NULL) {
            semaphores[i/16] = calloc(16,sizeof(semaphore));    /* Create a new block if necessary */
            if (semaphores[i/16] == NULL) {                     /* Out of memory, return! */
                return 1;
            }
        }

        if (!(semaphores[i/16][i%16].flags & SEMAPHORE_ACTIVE)) {
            semaphores[i/16][i%16].flags |= SEMAPHORE_ACTIVE;
            *sem = i+1; /* i+1 because 0 should always be an invalid semaphore handle */
            return 0;
        }
    }

    return 2; /* There is no more room for semaphores */
}

unsigned int ROSA_semaphoreTake(semHandle sTakeHandle) {
    semaphore* sem = ROSA_prvSemaphoreGet(sTakeHandle);
    tcb* task = ROSA_prvGetFirstFromReadyQueue();
    semaphore_reglist* it = NULL;
    
    if (sem == NULL) {
        return 1;
    }
    
    if (sem->owner != NULL) {       /* This should never happen */
        return 2;
    }

	/* DEBUG Delete these codes once semaphore registration works */

	/*sem->owner = task;
	ROSA_prvRaiseTaskPriority(task, sem->reglist->task->priority);
	return 0;*/

	/* END DEBUG */

    for (it=sem->reglist; it != NULL; it = it->next) {
        if (it->task == task) {
            sem->owner = task;
            ROSA_prvRaiseTaskPriority(task, sem->reglist->task->priority);
            return 0;
        }
    }

    return 3;                       /* Not registered to semaphore */
}

unsigned int ROSA_semaphoreGive(semHandle sGiveHandle) {
    semaphore* sem = ROSA_prvSemaphoreGet(sGiveHandle);
    
    if (sem == NULL) {
        return 1;
    }
    
    if (sem->owner == NULL) {
        return 2;
    }

    if (sem->owner != ROSA_prvGetFirstFromReadyQueue()) {
        return 3;
    }

    ROSA_prvResetTaskPriority(sem->owner);
    sem->owner = NULL;

    return 0;
}

/* Private function definitions */

int ROSA_prvSemaphoreIsTakenByTask(semHandle s, tcb* task) {
	semaphore* sem = ROSA_prvSemaphoreGet(s);
	
	if (sem->owner == task) {
		return 1;
	}
	
	return 0;
}

int ROSA_prvSemaphoreRegister(semHandle s, tcb* task) {
    semaphore* sem = ROSA_prvSemaphoreGet(s);
	semaphore_reglist *temp = sem->reglist;
	semaphore_reglist *ptr;

	if (sem == NULL) {
		return 1;
	}

	ptr = (semaphore_reglist *)calloc(1, sizeof(semaphore_reglist));

	if (ptr == NULL) return 2;

	ptr->task = task;
	//ptr->next = NULL;

	if (temp == NULL || ptr->task->priority > temp->task->priority)
	{					//Executes when linked list is empty 
		ptr->next = sem->reglist;
		sem->reglist = ptr;
		return 0;
	}

	while (temp->next != NULL) {
		if (ptr->task->priority > temp->next->task->priority) {
			ptr->next = temp->next;
			temp->next = ptr;
			return 0;
		}
		temp = temp->next;
	}
	
	temp->next = ptr;
	return 0;
}

int ROSA_prvSemaphoreUnregister(semHandle s, tcb* task) {
    semaphore* sem = ROSA_prvSemaphoreGet(s);
	semaphore_reglist *temp1 = sem->reglist;
	semaphore_reglist *ptr = NULL;
	int counter=0;
	
	if (temp1 == NULL) {
		return 1;
	}

	if (temp1->task == task)
	{
		ptr = temp1;
		sem->reglist = temp1->next;
		free(ptr);
		
		if (sem->owner == task) {
			sem->owner = NULL;
		}
		
		
		for (temp1=sem->reglist; temp1 != NULL; temp1 = temp1->next) {
			counter++;
		}

	    usartWriteChar(USART, '0'+counter);
		
		return 0;
	}

	do {
		if (temp1->next->task == task)
		{
			ptr = temp1->next;
			temp1->next = temp1->next->next;
			free(ptr);

			if (sem->owner == task) {
				sem->owner = NULL;
			}
			
			for (temp1=sem->reglist; temp1 != NULL; temp1 = temp1->next) {
				counter++;
			}

			usartWriteChar(USART, '0'+counter);

			return 0;
		}

		temp1 = temp1->next;
	} while (temp1->next != NULL);

	return 2;
}

semaphore* ROSA_prvSemaphoreGet(semHandle sem) {
	int i,j;
	i= ((sem-1)/16)%16;
    if (semaphores[((sem-1)/16)%16] == NULL) { /* Semaphore not yet created */
        return NULL;
    }

	j = (sem-1)%16;
    if (!(semaphores[((sem-1)/16)%16][(sem-1)%16].flags & SEMAPHORE_ACTIVE)) { /* The semaphore has not been initialised */
        return NULL;
    }

    return &semaphores[(sem-1)/16][(sem-1)%16];
}

int ROSA_prvForceGiveSemaphore(semHandle s, tcb* task){
	semaphore* sem = ROSA_prvSemaphoreGet(s);
	
	sem->owner = NULL;
	ROSA_prvResetTaskPriority(task);
	
	return 0;
	
}