#ifndef ROSA_DEBUG_H_
#define ROSA_DEBUG_H_

#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"


tcb debug_tcb;


#define DEBUG_STACK_SIZE 0x100
static int debug_stack[DEBUG_STACK_SIZE];
unsigned int absolute_delay;

void debug_task(void);
void debug_init(unsigned int period);



#endif /* ROSA_DEBUG_H_ */