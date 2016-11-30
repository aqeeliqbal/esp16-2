#include "kernel/rosa_sem.h"
#include "kernel/rosa_"

/* Constants */

#define SEMAPHORE_ACTIVE (1 << 0)
#define SEMAPHORE_LOCKED (1 << 1)

/* Structs */

/*
  The struct for each post in the registration list

  prio - the priority of the task
  task - the task registered to this semaphore
  next - next post in list
*/

typedef struct sem_reglist_record_t {
    uint_8 prio;
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
    uint8 flags;
    tcb* owner;
    semaphore_reglist* reglist;
} semaphore;

/* Private function declarations */

void ROSA_SemaphoreRegister(semHandle sem);
void ROSA_SemaphoreUnregister(semHandle sem);
semaphore* ROSA_SemaphoreGet(semHandle sem);

/* Global variables not accessible outside this file */

semaphore* semaphores[16];

/* Public function definitions */

unsigned int ROSA_semaphoreCreate(semHandle *sem) {
    uint16 i = 0;                   /* sem will receive a value between 1 and 256 */

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
    sem* = ROSA_semaphoreGet(sTakeHandle);

    if (sem == NULL) {
        return 1;
    }
    
    if (sem->owner != NULL) {
        return 2;
    }
    
    sem->owner = ROSA_schedulerGetCurrentTask();
    ROSA_schedulerRaisePriority(sem->owner,sem->reglist->prio);

    return 0;
}

unsigned int ROSA_semaphoreGive(semHandle sGiveHandle) {
    semaphore* sem = ROSA_semaphoreGet(sTakeHandle);
    
    if (sem == NULL) {
        return 1;
    }
    
    if (sem->owner == NULL) {
        return 2;
    }

    if (sem->owner != ROSA_schedulerGetCurrentTask()) {
        return 3;
    }

    ROSA_schedulerResetPriority(sem->owner);
    sem->owner = NULL;

    return 0;
}

/* Private function definitions */

void ROSA_SemaphoreRegister(semHandle sem) {

}

void ROSA_SemaphoreUnregister(semHandle sem) {

}

semaphore* ROSA_SemaphoreGet(semHandle sem) {
    if (semaphores[((sem%-1)/16)%16] == NULL) { /* Semaphore not yet created */
        return NULL;
    }

    if (semaphores[((sem%-1)/16)%16][sem-1%16].flags & SEMAPHORE_ACTIVE) { /* The semaphore has not been initialised */
        return NULL;
    }

    return &semaphores[(sem-1)/16][(sem-1)%16];
}
