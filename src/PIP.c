/*
 * PIP.c
 *
 * Created: 2017-01-12 14:33:45
 *  Author: aah16002
 */ 
 #include "kernel/rosa_ker.h"
 #include "kernel/rosa_scheduler.h"
 #include "kernel/rosa_sem.h"

 #include "drivers/led.h"
 #include <avr32/io.h>
 #include "rosa_config.h"

 semHandle sempip[3] = {0};

 #define MY_STACK_SIZE 0x100
 static int t1_stack[MY_STACK_SIZE];
 static int t2_stack[MY_STACK_SIZE];
  static int t3_stack[MY_STACK_SIZE];
 //static const char *texttest = "Task1 is running\n";

 void task_pip1(void* tcbArg) {
	 //char * taskname;
	 //taskname=(char*)tcbArg;
	 //usartWriteLine(USART, taskname);
	 ticktime ticks = ROSA_getTicks();
	 int i;
	 int tmp;

	 while (1) {
		 // Toggle LED1
		 for (i=0; i<300000; i++) {
			 ledOff(LED0_GPIO);
			 ledOn(LED0_GPIO);
			 if (i == 100000) {
				 while(ROSA_semaphoreTake(sempip[1]));
				 ledOn(LED4_GPIO);
				 } else if (i == 200000) {
				 ledOff(LED4_GPIO);
				 ROSA_semaphoreGive(sempip[1]);
			 }
		 }
		 
		 ledOff(LED0_GPIO);

		 // Sleep for 1000 ms
		 ROSA_taskDelayUntil(&ticks,5000);
	 }
 }

 void task_pip2(void* tcbArg) {
	 ticktime ticks;
	 int i;

	 ROSA_taskDelay(2500);
	 ticks =  ROSA_getTicks();

	 while (1) {
		 // Toggle LED2
		 
		 for (i=0; i<300000; i++) {
			 //ledOff(LED1_GPIO);
			 ledOn(LED2_GPIO);
			 if (i == 100000) {
				 //ROSA_semaphoreTake(sem[1]);
				 ledOn(LED3_GPIO);
				 } else if (i == 200000) {
				 ledOff(LED3_GPIO);
				 //ROSA_semaphoreGive(sem[1]);
			 }
		 }

		 ledOff(LED2_GPIO);

		 // Sleep for 1000 ms
		 ROSA_taskDelayUntil(&ticks,10000);
	 }
 }
  void task_pip3(void* tcbArg) {
	  ticktime ticks;
	  int i;

	  ROSA_taskDelay(1500);
	  ticks =  ROSA_getTicks();

	  while (1) {
		  // Toggle LED2
		  
		  for (i=0; i<300000; i++) {
			  //ledOff(LED1_GPIO);
			  ledOn(LED1_GPIO);
			  if (i == 100000) {
				  while(ROSA_semaphoreTake(sempip[1]));
				  ledOn(LED6_GPIO);
				  } else if (i == 200000) {
				  ledOff(LED6_GPIO);
				  ROSA_semaphoreGive(sempip[1]);
			  }
		  }

		  ledOff(LED1_GPIO);

		  // Sleep for 1000 ms
		  ROSA_taskDelayUntil(&ticks,5000);
	  }
  }

 void semaphorePIPTest() {
	 tcbHandle t1Handle;
	 tcbHandle t2Handle;
	 tcbHandle t3Handle;
	 
	 ROSA_Extended_Init();
	 
	 
	 ROSA_semaphoreCreate(&sempip[0]);
	 ROSA_semaphoreCreate(&sempip[1]);
	 ROSA_semaphoreCreate(&sempip[2]);
	 
	 ROSA_tcbCreate(&t1Handle,"task_pip1",task_pip1,t1_stack,MY_STACK_SIZE,1,NULL,sempip,3);
	 ROSA_tcbCreate(&t2Handle,"task_pip2",task_pip2,t2_stack,MY_STACK_SIZE,2,NULL,NULL,0);
	 ROSA_tcbCreate(&t3Handle,"task_pip3",task_pip3,t3_stack,MY_STACK_SIZE,3,NULL,&sempip[1],2);
	 //Deletes task 1
	 //ROSA_tcbDelete(&t2Handle);
	 
	 ROSA_Extended_Start();
 }