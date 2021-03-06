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
	if ( ticks >= MAX_TICK_COUNT){
		ticks = ticks - MAX_TICK_COUNT;
	}
	
	ROSA_prvclockTickCompare();
	
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
	//interruptDisable();
	
	//while(WAITINGQUEUE->count > 0 && WAITINGQUEUE->heaparr[0].value <= ticks){
	while(ROSA_prvGetFirstWakeTime() <= ticks){
		ROSA_prvAddToReadyQueue(ROSA_prvGetFirstFromWaitingQueue());
		//ROSA_prvAddToReadyQueue(WAITINGQUEUE->heaparr[0].task_tcb);
		ROSA_prvRemoveFromWaitingQueue(ROSA_prvGetFirstFromWaitingQueue());
		//ROSA_prvRemoveFromWaitingQueue(WAITINGQUEUE->heaparr[0].task_tcb);
	}
	
	//interruptEnable();
	return 0;
}


int ROSA_prvClockOverflow(ticktime *start){
	
	ticktime min;
	if ( ROSA_prvGetFirstWakeTime() < *start){ //if start is bigger, then clock must be bigger. interrupt causes
		min = ROSA_prvGetFirstWakeTime();
	}
	else{
		min = *start;
	}
	ROSA_prvDecreasetWaitingQueueValues(min);
	*start = *start - min;
	ticks = ticks - min;
	
	return 0;
}


/******************************************
DelayUntil
******************************************/
//0 = fine
//1 = manipulation of getTick
//2 = invalid period set
//3 = delay time is past by the clock

int ROSA_taskDelayUntil(ticktime *start, ticktime t){             
	tcb *readyP = NULL;
	ticktime maxClock = MAX_TICK_COUNT;
		
	ticktime rest = maxClock - *start; // the remaining time before maxClock
	int err;

	if ((*start > ROSA_getTicks()) || *start >= maxClock ){ //avoid user mistake to send a bigger/negative number.
		return 1;
	}
	if (t >= maxClock){ //results in cyclic overflow
		return 2;
	}
	if (t >= rest){  //valid overflow, t is not bigger than maxClock since that is checked already
		ROSA_prvClockOverflow(start); //subtract clk with start, subtracting tasks with clk could cause neg. values if interrupt disable 
	}
	if ((*start + t) <= ROSA_getTicks()){ //task code takes longer time than the delay period
		return 3;
	}
	*start = *start + t;   //add start tick value and period to get the delay time
	interruptDisable();
	readyP = ROSA_prvGetFirstFromReadyQueue();  //task that will be delayed
	err = ROSA_prvRemoveFromReadyQueue(readyP);
		
	//usartWriteChar(USART, err + '0');
	ROSA_prvAddToWaitingQueue(readyP, *start);
	interruptEnable();
		
	ROSA_yield();

	return 0;
		
}


/*******************************************

Delay
*******************************************/
//0 = fine
//1 = invalid period set 

int ROSA_taskDelay(ticktime t){   //if delayUntil returns other than zero, return error
	ticktime wake = ROSA_getTicks();
	if(ROSA_taskDelayUntil(&wake, t)){
		return 1;
	}
	return 0;
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
