/*
 * rosa_idle.h
 *
 * Created: 2016-12-08 17:47:30
 *  Author: snubbe
 */ 


#ifndef ROSA_IDLE_H_
#define ROSA_IDLE_H_

#include "drivers/led.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"


static tcb idle_tcb;


#define IDLE_STACK_SIZE 0x100
static int idle_stack[IDLE_STACK_SIZE];

void idle(void);

#endif /* ROSA_IDLE_H_ */