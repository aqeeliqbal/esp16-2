#include "../include/kernel/rosa_sem.h"
#include "../include/kernel/rosa_ker.h"
#include <stdlib.h>
/*#include <stdio.h>*/

/* Constants */

#define SEMAPHORE_ACTIVE (1 << 0)
#define SEMAPHORE_LOCKED (1 << 1)

/* DELETE THIS SECTION LATER - COMMON PRIVATE FUNCTIONS

typedef struct t {
    uint8_t prio;
    uint8_t curPrio;
} tcb;

tcb* ROSA_schedulerGetCurrentTask() {
    static tcb task = { 3, 0 };
    return &task;
}

void ROSA_schedulerRaisePriority(tcb* task, int prio) {
    task->curPrio = prio;
}

void ROSA_schedulerResetPriority(tcb* task) {
    task->curPrio = task->prio;
}


/* END OF SECTION */

/* Structs */

/*
  The struct for each post in the registration list

  prio - the priority of the task
  task - the task registered to this semaphore
  next - next post in list
*/

typedef struct sem_reglist_record_t {
    uint8_t prio;
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

/* Private function declarations */

void ROSA_prvSemaphoreRegister(semHandle sem, tcb* task);
void ROSA_prvSemaphoreUnregister(semHandle sem, tcb* task);
semaphore* ROSA_prvSemaphoreGet(semHandle sem);

/* Global variables not accessible outside this file */

semaphore* semaphores[16];

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
    semaphore* sem = ROSA_semaphoreGet(sTakeHandle);
    tcb* task = ROSA_prvGetCurrentTask();
    semaphore_reglist* it = NULL;
    
    if (sem == NULL) {
        return 1;
    }
    
    if (sem->owner != NULL) {       /* This should never happen */
        return 2;
    }

    for (it=sem->regList; it != NULL; it = it->next) {
        if (it->task == task) {
            sem->owner = task;
            ROSA_prvRaiseTaskPriority(task, sem->reglist->task->prio);
            return 0;
        }
    }

    return 3;                       /* Not registered to semaphore */
}

unsigned int ROSA_semaphoreGive(semHandle sGiveHandle) {
    semaphore* sem = ROSA_semaphoreGet(sGiveHandle);
    
    if (sem == NULL) {
        return 1;
    }
    
    if (sem->owner == NULL) {
        return 2;
    }

    if (sem->owner != ROSA_schedulerGetCurrentTask()) {
        return 3;
    }

    ROSA_prvResetTaskPriority(sem->owner);
    sem->owner = NULL;

    return 0;
}

/* Private function definitions */

void ROSA_SemaphoreRegister(semHandle sem, tcb* task) {

}

void ROSA_SemaphoreUnregister(semHandle sem, tcb* task) {

}

semaphore* ROSA_semaphoreGet(semHandle sem) {
    if (semaphores[((sem%-1)/16)%16] == NULL) { /* Semaphore not yet created */
        return NULL;
    }

    if (semaphores[((sem%-1)/16)%16][sem-1%16].flags & SEMAPHORE_ACTIVE) { /* The semaphore has not been initialised */
        return NULL;
    }

    return &semaphores[(sem-1)/16][(sem-1)%16];
}

/* DELETE EVERYTHING FROM THIS LINE FORWARD

void print_sem(semHandle sem) {
    semaphore* s = ROSA_semaphoreGet(sem);
    printf("semaphore %d: %d,%d\n",(int)s,(int)s->owner,s->flags);
}

void print_task(tcb* task) {
    printf("task: %d, %d,\n",task->prio,task->curPrio);
}

int main(void) {
    semHandle sem[258];
    int res,i;

    for (i=0; i<258; i++) {
        res = ROSA_semaphoreCreate(&sem[i]);
        printf("Create Semaphore %d: %d %d\n",i+1,res,sem[i]);
    }

    for (i=240; i<258; i++) {
        
    }
    
    
    return 0;
} /* */
