#ifndef __ROSA_SEM_H__
#define __ROSA_SEM_H__

typedef uint_16 semHandle;

unsigned int ROSA_semaphoreCreate(semHandle *sem);
unsigned int ROSA_semaphoreTake(semHandle sTakeHandle);
unsigned int ROSA_semaphoreGive(semHandle sGiveHandle);

#endif
