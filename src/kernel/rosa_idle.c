/*
 * rosa_idle.c
 *
 * Created: 2016-12-08 17:46:56
 
 */ 
#include "drivers/led.h"
#include "rosa_idle.h"
#include "kernel/rosa_ker.h"

int state = 0;
void idle(void){
	
	while(1){

		if(state == 0){
			state = 1;
			ledOn(LED2_GPIO);
		}
		else{
			state = 0;
			ledOff(LED2_GPIO);
		}
		ticktime start = ROSA_getTicks();
		while(ROSA_getTicks() < start + 1000);
		//while(WAITINGQUEUE->count > 0 && ROSA_prvGetFirstWakeTime() <= ROSA_getTicks()){
			//int err;
			//err = ROSA_prvAddToReadyQueue(ROSA_prvGetFirstFromWaitingQueue());
			////usartWriteChar(USART, err+'0');
			//err = ROSA_prvRemoveFromWaitingQueue(ROSA_prvGetFirstFromWaitingQueue());
			////usartWriteChar(USART, err+'0');
		//}
		
		
		//usartWriteTcb(USART, ROSA_prvGetFirstFromWaitingQueue());
		
		usartWriteChar(USART, 'T');
		queue_display(READYQUEUE);

		
		ROSA_yield();
	}
}
