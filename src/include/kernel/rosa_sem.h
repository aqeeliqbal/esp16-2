#ifndef __ROSA_SEM_H__
#define __ROSA_SEM_H__

#include <stdint.h>
#include "kernel/rosa_def.h"

unsigned int ROSA_semaphoreCreate(semHandle *sem);
unsigned int ROSA_semaphoreTake(semHandle sTakeHandle);
unsigned int ROSA_semaphoreGive(semHandle sGiveHandle);

/* Private functions */

int ROSA_prvSemaphoreRegister(semHandle sem, tcb* task);
int ROSA_prvSemaphoreUnregister(semHandle sem, tcb* task);

int ROSA_prvForceGiveSemaphore(semHandle s, tcb* task);
int ROSA_prvSemaphoreIsTakenByTask(semHandle s, tcb* task);
int ROSA_prvSemCheckInReglist(semHandle s, tcb* task);

#endif
