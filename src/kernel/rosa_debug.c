#include "kernel/rosa_debug.h"
#include "drivers/usart.h"
#include "drivers/button.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_queue.h"

void debug_task(void){
	
	while(1){
		if(isButton(PUSH_BUTTON_0)){
			interruptDisable();
			usartWriteLine(USART, "___________________________________\n\r");
			usartWriteLine(USART, "Clock: ");
			display_number(ROSA_getTicks());
			tcb* dbg = ROSA_prvGetFirstFromReadyQueue();
			ROSA_prvRemoveFromReadyQueue(dbg);
			usartWriteLine(USART, "\n\rREADY QUEUE:\n\r");
			queue_display(READYQUEUE);
			usartWriteLine(USART, "-----------------------------------\n\r");
			
			ROSA_prvAddToReadyQueue(dbg);
			interruptEnable();
			
			usartWriteLine(USART, "WAITING QUEUE:\n\r");
			queue_display(WAITINGQUEUE);
			usartWriteLine(USART, "___________________________________\n\r");
			
		}
		
		ROSA_taskDelay(absolute_delay);
	}
}

void debug_init(unsigned int period){
	if(rosa_initiated != 1){
		return;
	}
	absolute_delay = period;
	ROSA_tcbCreate(&debug_tcb, "dbg", debug_task, debug_stack, DEBUG_STACK_SIZE, MAX_TASK_PRIORITY, NULL, NULL, 0);
}