 
#ifndef __ROSA_SEM_H__
#define __ROSA_SEM_H__

#include <stdint.h>
#include "kernel/rosa_def.h"


//typedef uint16_t semHandle;

/* Private function declarations */

int ROSA_prvSemaphoreRegister(semHandle sem, tcb* task);
int ROSA_prvSemaphoreUnregister(semHandle sem, tcb* task);

#endif