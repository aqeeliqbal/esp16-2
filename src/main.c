/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// ///''\\,
             ,///' '///,'/////',/////'  /////'\\,

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

//Standard library includes
//#include <avr32/io.h>
//
////Kernel includes
//#include "kernel/rosa_ker.h"
//
////Driver includes
//#include "drivers/led.h"
//#include "drivers/delay.h"
//#include "drivers/usart.h"
//#include "kernel/rosa_sem.h"
//#include "kernel/rosa_prv.h"
#include "test.h"
//
////Include configuration
//#include "rosa_config.h"
//
////Data blocks for the tasks
////#define T1_STACK_SIZE 0x40
//#define T1_STACK_SIZE 0x100
//static int t1_stack[T1_STACK_SIZE];
////static tcb t1_tcb;
//
//#define T2_STACK_SIZE 0x100
//static int t2_stack[T2_STACK_SIZE];
////static tcb t2_tcb;
//tcbHandle t1_tcb ;
//tcbHandle t2_tcb ;


int main(void)
{
	semaphoreTest();
	//testcase2_3();
	
	while(1);
	return 0;
}