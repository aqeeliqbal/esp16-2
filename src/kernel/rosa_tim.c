/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///' '///,'/////',/////'  /////'/;,

    Copyright 2010 Marcus Jansson <mjansson256@yahoo.se>

    This file is part of ROSA - Realtime Operating System for AVR32.

    ROSA is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROSA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ROSA.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
/* Tab size: 4 */

#include "rosa_config.h"
#include "drivers/delay.h"
#include "drivers/led.h"
#include "kernel/rosa_int.h"

#include "kernel/rosa_def.h"
#include "kernel/rosa_ker.h"
#include "stdint.h" //for uint32_t

ticktime ticks = 0;

int turnOn(int led){
	//ROSA_prvRemoveFromReadyQueue(&idle_tcb);
	ledOn(led);
	return 1;
}

/***********************************************************
 * timerInterruptHandler
 *
 * Comment:
 * 	This is the basic timer interrupt service routine.
 **********************************************************/
int value;
__attribute__((__interrupt__))
void timerISR(void)
{
	int sr;
	volatile avr32_tc_t * tc = &AVR32_TC;
	ticks++;
	//if(ROSA_prvGetFirstFromReadyQueue() == NULL)
		//ROSA_prvAddToReadyQueue(&idle_tcb);
	
	//This one stops it from working if it is local, but works if it is global
	//value = turnOn(LED3_GPIO);
	//This one is OK
	//turnOn(LED3_GPIO);
	
	ROSA_prvclockTickCompare();
	/*
	// This WORKED
	while(WAITINGQUEUE->count > 0 && WAITINGQUEUE->heaparr[0].value <= ticks){
		//ledOn(LED3_GPIO);
		ROSA_prvAddToReadyQueue(WAITINGQUEUE->heaparr[0].task_tcb);
		ROSA_prvRemoveFromWaitingQueue(WAITINGQUEUE->heaparr[0].task_tcb);
	}*/  /*
	else{
		ledOff(LED3_GPIO);
	} */
	
	//Read the timer status register to determine if this is a valid interrupt
	sr = tc->channel[0].sr;
	if(sr & AVR32_TC_CPCS_MASK){
		ROSA_yieldFromISR();
	}
}


/********************************************
ROSA_getTicks
get the current clock ticks

********************************************/

ticktime ROSA_getTicks(void){
	
	return ticks;
}


/******************************************
clockTicksCompare
******************************************/


int ROSA_prvclockTickCompare(void){  //put this in ISR or before scheduler

	//tcb *waitQp = NULL;
	//ticktime curr = ROSA_getTicks();
	//int err;		
	interruptDisable();
	//if (curr >= ROSA_prvGetFirstFromWaitingQueue->value){
	
		while(WAITINGQUEUE->count > 0 && WAITINGQUEUE->heaparr[0].value <= ticks){
			
		ROSA_prvAddToReadyQueue(WAITINGQUEUE->heaparr[0].task_tcb);
		ROSA_prvRemoveFromWaitingQueue(WAITINGQUEUE->heaparr[0].task_tcb);
		}
	
	
	/*while(curr >= ROSA_prvGetFirstWakeTime()){  //check if next has the same delay and so on
		waitQp = ROSA_prvGetFirstFromWaitingQueue();
		if (waitQp == NULL){
			break;
		}
		err = ROSA_prvRemoveFromWaitingQueue(waitQp);
		ROSA_prvAddToReadyQueue(waitQp);
	}*/
	//}
	
	interruptEnable();
	return 0;
}

//int ROSA_prvclockTickCompare(void){
	//ticktime current_time = ROSA_getTicks();
	//
	//while(current_time >= ROSA_prvGetFirstWakeTime()){
		//tcb * readyTask = ROSA_prvGetFirstFromWaitingQueue();
		//if(readyTask == NULL){
			//break;
		//}
		//ROSA_prvRemoveFromWaitingQueue(readyTask);
		//ROSA_prvAddToReadyQueue(readyTask);
	//}
	//return 0;
//}


/******************************************
DelayUntil
******************************************/
//0 = fine
//1 = over overflow limit


int ROSA_taskDelayUntil(ticktime start, ticktime t){
	tcb *readyP = NULL;
	ticktime maxClock = 4294967295;
		ticktime sum = start + t;
		ticktime rest = maxClock - start;
		int err;
	/*	if (t > 3900000000){
			return 1;
		}
		else if (t > rest) && ( t< 4294967295){
			ROSA_clockOverflow();
		}
		else if (t < rest) && ((start + t) < now=ROSA_getTicks()){
			ROSA_prvAddToReadyQueue(&EXECTASK);
		}
		else{*/
		//interruptDisable();
		//void contextSave();
		readyP = ROSA_prvGetFirstFromReadyQueue();
		err = ROSA_prvRemoveFromReadyQueue(readyP);
		
		//usartWriteChar(USART, err + '0');
		ROSA_prvAddToWaitingQueue(readyP, sum);
		//void contextRestore();
		//interruptEnable();
		ROSA_yield();
		return 0;
		//}
}

//int ROSA_taskDelayUntil(ticktime start, ticktime t){
	//ticktime new_wake_time = start + t;
	//
	//tcb * readyTask = ROSA_prvGetFirstFromReadyQueue();
	//ROSA_prvRemoveFromReadyQueue(readyTask);
	//ROSA_prvAddToWaitingQueue(readyTask, new_wake_time);
	//ROSA_yield();
	//
	//return 0;
//}


/*******************************************

Delay
*******************************************/


int ROSA_taskDelay(ticktime t){
	ticktime wake = ROSA_getTicks();

	ROSA_taskDelayUntil(wake, t);
}


/***********************************************************
 * timerPeriodSet
 *
 * Comment:
 * 	Set the timer period to 'ms' milliseconds.
 *
 **********************************************************/
int timerPeriodSet(unsigned int ms)
{

	int rc, prescale;
	int f[] = { 2, 8, 32, 128 };
	//FOSC0 / factor_prescale * time[s];
	prescale = AVR32_TC_CMR0_TCCLKS_TIMER_CLOCK5;
	rc = FOSC0 / f[prescale - 1] * ms / 1000;
	timerPrescaleSet(prescale);
	timerRCSet(rc);
	return rc * prescale / FOSC0;
}
