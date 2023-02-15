/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "queue.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


#define BUTTON1_PORT		PORT_0
#define BUTTON1_PIN			PIN0

#define BUTTON2_PORT		PORT_0
#define BUTTON2_PIN			PIN1

#define BUTTON1_PERIOD				50
#define BUTTON2_PERIOD				50
#define PERIODIC_PERIOD		  	100
#define UART_PERIOD					  20
#define LOAD1_PERIOD					10
#define LOAD2_PERIOD					100

unsigned int Button_1_Start, Button_1_Total;
unsigned int Button_2_Start, Button_2_Total;
unsigned int Periodic_Transmitter_Start, Periodic_Transmitter_Total;
unsigned int Uart_Receiver_Start, Uart_Receiver_Total;
unsigned int Load_1_Start, Load_1_total;
unsigned int Load_2_Start, Load_2_total;
unsigned int System_Time;
unsigned int Cpu_Load;									

BaseType_t xReturned;
QueueHandle_t xMessage_Buffer1 = NULL;

TaskHandle_t Button_1_Monitor_Handler = NULL;
TaskHandle_t Button_2_Monitor_Handler = NULL;
TaskHandle_t Periodic_Transmitter_Handler = NULL;
TaskHandle_t Uart_Receiver_Handler = NULL;
TaskHandle_t Load_1_Simulation_Handler = NULL;
TaskHandle_t Load_2_Simulation_Handler = NULL;

/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/
void Button_1_Monitor( void * pvParameters );
void Button_2_Monitor( void * pvParameters );
void Periodic_Transmitter (void * pvParameters );
void Uart_Receiver (void * pvParameters );
void Load_1_Simulation ( void * pvParameters );
void Load_2_Simulation ( void * pvParameters );

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
    /* Create Tasks here */
	xReturned = xTaskPeriodicCreate(
                    Button_1_Monitor,       /* Function that implements the task. */
                    "BM1",     /* Text name for the task. */
                    100,      				/* Stack size in words, not bytes. */
                    ( void * ) 0,    		/* Parameter passed into the task. */
                    1,						/* Priority at which the task is created. */
                    &Button_1_Monitor_Handler		/* Used to pass out the created task's handle. */
										,BUTTON1_PERIOD);      				/* Task Deadline */
					
	xReturned = xTaskPeriodicCreate(
                    Button_2_Monitor,       /* Function that implements the task. */
                    "BM2",     /* Text name for the task. */
                    100,      				/* Stack size in words, not bytes. */
                    ( void * ) 0,    		/* Parameter passed into the task. */
                    1,						/* Priority at which the task is created. */
                    &Button_2_Monitor_Handler,		/* Used to pass out the created task's handle. */
										BUTTON2_PERIOD);      				/* Task Deadline */
	    				
	xReturned = xTaskPeriodicCreate(
                    Periodic_Transmitter,       /* Function that implements the task. */
                    "PT",     /* Text name for the task. */
                    100,      				/* Stack size in words, not bytes. */
                    ( void * ) 0,    		/* Parameter passed into the task. */
                    1,						/* Priority at which the task is created. */
                    &Periodic_Transmitter_Handler,		/* Used to pass out the created task's handle. */
										PERIODIC_PERIOD);															/* Task Deadline */  
	
	xReturned = xTaskPeriodicCreate(
                    Uart_Receiver,       /* Function that implements the task. */
                    "UR",     /* Text name for the task. */
                    100,      				/* Stack size in words, not bytes. */
                    ( void * ) 0,    		/* Parameter passed into the task. */
                    1,						/* Priority at which the task is created. */
                    &Uart_Receiver_Handler,		/* Used to pass out the created task's handle. */
										UART_PERIOD);      				/* Task Deadline */
	
	xReturned = xTaskPeriodicCreate(
                    Load_2_Simulation,       /* Function that implements the task. */
                    "LS2",     /* Text name for the task. */
                    100,      				/* Stack size in words, not bytes. */
                    ( void * ) 0,    		/* Parameter passed into the task. */
                    1,						/* Priority at which the task is created. */
                    &Load_2_Simulation_Handler,		/* Used to pass out the created task's handle. */
										LOAD2_PERIOD);      				/* Task Deadline */
		
	xReturned = xTaskPeriodicCreate(
                    Load_1_Simulation,       /* Function that implements the task. */
                    "LS1",     /* Text name for the task. */
                    100,      				/* Stack size in words, not bytes. */
                    ( void * ) 0,    		/* Parameter passed into the task. */
                    1,						/* Priority at which the task is created. */
                    &Load_1_Simulation_Handler,		/* Used to pass out the created task's handle. */
										LOAD1_PERIOD);      				/* Task Deadline */
					
	xMessage_Buffer1 = xQueueCreate( 3, sizeof(char));
		
	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();
	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook (void)
{
	GPIO_write(PORT_1 , PIN1, PIN_IS_HIGH);
	GPIO_write(PORT_1 , PIN1, PIN_IS_LOW);
}

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000; //20 kHhZ
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


/*Tasks Implemntations*/ 
void Button_1_Monitor( void * pvParameters )
{
	uint32_t xLastWakeTime = xTaskGetTickCount();
	char Edge_Flag = 0;

	for( ;; )
	{
		pinState_t Current_State = GPIO_read(BUTTON1_PORT,BUTTON1_PIN);
		if( Current_State == PIN_IS_HIGH )
		{
			Edge_Flag = 'R';
		}
		else if (Current_State == PIN_IS_LOW )
		{
			Edge_Flag = 'F';
		}
		xQueueSend( xMessage_Buffer1  , &Edge_Flag ,(TickType_t) 0);
		vTaskDelayUntil( &xLastWakeTime , BUTTON1_PERIOD);
	}
}

void Button_2_Monitor( void * pvParameters )
{
	uint32_t xLastWakeTime = xTaskGetTickCount();
	char Edge_Flag = 0;
	for( ;; )
	{
		pinState_t Current_State = GPIO_read(BUTTON2_PORT,BUTTON2_PIN);
		if( Current_State == PIN_IS_HIGH )
		{
			Edge_Flag = 'r';
		}
		else if (Current_State == PIN_IS_LOW)
		{
			Edge_Flag = 'f';
		}
		xQueueSend( xMessage_Buffer1  , &Edge_Flag ,(TickType_t) 0);
		vTaskDelayUntil( &xLastWakeTime , BUTTON2_PERIOD);
	}
}

void Periodic_Transmitter (void * pvParameters )
{
	uint32_t xLastWakeTime = xTaskGetTickCount();
	char Edge_Flag = 0;
	for( ; ; )
	{
		 if(( uxQueueSpacesAvailable( xMessage_Buffer1 ) ) > 0 && (xMessage_Buffer1 != NULL) )
		{
			Edge_Flag = 'P';
			xQueueSend(xMessage_Buffer1,&Edge_Flag,PERIODIC_PERIOD ); 
		}
		vTaskDelayUntil( &xLastWakeTime , PERIODIC_PERIOD);
	}
}

void Uart_Receiver (void * pvParameters )
{
	uint32_t xLastWakeTime = xTaskGetTickCount();
	char Received_message1;
	char Received_message2;
	char Received_message3;

	for( ; ; )
	{
		if( xQueueReceive( xMessage_Buffer1, &(Received_message1) , 0) && ((Received_message1 == 'R') ||(Received_message1 == 'F')))
		{
			xSerialPutChar('B');
			xSerialPutChar('T');
			xSerialPutChar('1');
			xSerialPutChar('=');
			xSerialPutChar(Received_message1);
      xSerialPutChar('\n');		
		}
		else
		{	
			xSerialPutChar(' ');
			xSerialPutChar(' ');
			xSerialPutChar(' ');
		}
		if( xQueueReceive( xMessage_Buffer1, &(Received_message2) , 0) && ((Received_message2 == 'r') ||(Received_message2 == 'f')))
		{
			xSerialPutChar('B');
			xSerialPutChar('T');
			xSerialPutChar('2');
			xSerialPutChar('=');
			xSerialPutChar(Received_message2);
		  xSerialPutChar('\n');
		}
		else
		{	
			xSerialPutChar(' ');
			xSerialPutChar(' ');
			xSerialPutChar(' ');
		}
		if( xQueueReceive( xMessage_Buffer1, &(Received_message3) , 0) && (Received_message3 == 'P'))
		{
			xSerialPutChar('P');
			xSerialPutChar('T');
			xSerialPutChar('=');
			xSerialPutChar(Received_message3);
			xSerialPutChar('\n');
		}
		else
		{	
			xSerialPutChar(' ');
			xSerialPutChar(' ');
			xSerialPutChar(' ');
		}
		vTaskDelayUntil( &xLastWakeTime , UART_PERIOD);
	}
}

void Load_1_Simulation ( void * pvParameters )
{
	uint32_t xLastWakeTime = xTaskGetTickCount();
	uint32_t i = 0;
	uint32_t x = 12000*5;
	for( ; ; )
	{
		for( i=0 ; i <=x; i++)
		{
		}
		vTaskDelayUntil( &xLastWakeTime , LOAD1_PERIOD);
	}
}

void Load_2_Simulation ( void * pvParameters )
{
	uint32_t xLastWakeTime = xTaskGetTickCount();
	uint32_t i = 0;
	uint32_t x = 12000*12;
	for( ; ; )
	{		
		for( i=0 ; i <= x; i++)
		{
		}
		vTaskDelayUntil( &xLastWakeTime , LOAD2_PERIOD);
	}
}