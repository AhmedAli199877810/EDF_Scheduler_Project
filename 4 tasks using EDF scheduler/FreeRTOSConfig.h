/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
#include <stdint.h>
#include <lpc21xx.h>
#include "GPIO.h"
#include <string.h>

extern unsigned int Button_1_Start, Button_1_Total;
extern unsigned int Button_2_Start, Button_2_Total;
extern unsigned int Periodic_Transmitter_Start, Periodic_Transmitter_Total;
extern unsigned int Uart_Receiver_Start, Uart_Receiver_Total;
extern unsigned int Load_1_Start, Load_1_total;
extern unsigned int Load_2_Start, Load_2_total;
extern unsigned int System_Time;
extern unsigned int Cpu_Load;									

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/
 

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			1
#define configCPU_CLOCK_HZ			( ( unsigned long ) 60000000 )	/* =12.0MHz xtal multiplied by 5 using the PLL. */
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		( 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 90 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) 13 * 1024 )
#define configMAX_TASK_NAME_LEN		( 8 )
#define configUSE_TRACE_FACILITY	1
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1

//////////////////////////****************************/////////////////////
#define configUSE_EDF_SCHEDULER   1

#define configQUEUE_REGISTRY_SIZE 0
/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

#define SET_P0_PIN(PINx)		GPIO_write( PORT_0 , PINx , PIN_IS_HIGH)
#define SET_P1_PIN(PINx)		GPIO_write( PORT_1 , PINx , PIN_IS_HIGH)
#define CLR_P0_PIN(PINx)		GPIO_write( PORT_0 , PINx , PIN_IS_LOW)
#define CLR_P1_PIN(PINx)		GPIO_write( PORT_1 , PINx , PIN_IS_LOW)

#define traceTASK_SWITCHED_IN()			 if (strcmp(pxCurrentTCB->pcTaskName, "BM1" ) == 0 )\
																							{\
																								SET_P1_PIN(PIN0);\
																								Button_1_Start = T1TC;\
																							}\
										 if (strcmp(pxCurrentTCB->pcTaskName, "BM2" ) == 0 )\
																							{\
																							SET_P0_PIN(PIN2);\
																							Button_2_Start = T1TC;\
																							}\
										 if (strcmp(pxCurrentTCB->pcTaskName, "PT" ) == 0 )\
																							{\
																							SET_P0_PIN(PIN3);\
																							Periodic_Transmitter_Start = T1TC;\
																							}  	\
										 if (strcmp(pxCurrentTCB->pcTaskName, "UR" ) == 0 )\
																							{\
																							SET_P0_PIN(PIN4);\
																							Uart_Receiver_Start = T1TC;\
																							}  	\
										 if (strcmp(pxCurrentTCB->pcTaskName, "LS1" ) == 0 )\
																							{ \
																								SET_P0_PIN(PIN5);\
																								Load_1_Start = T1TC;\
																							}  	\
										 if (strcmp(pxCurrentTCB->pcTaskName, "LS2" ) == 0 )\
																							{\
																							SET_P0_PIN(PIN6);\
																							Load_2_Start = T1TC;\
																							}  	\
										 if (strcmp(pxCurrentTCB->pcTaskName, "IDLE" ) == 0 )\
																							{\
																							SET_P0_PIN(PIN7);\
																							}  				\
										 

#define traceTASK_SWITCHED_OUT()		 if (strcmp(pxCurrentTCB->pcTaskName, "BM1" ) == 0 )\
																						{\
																						CLR_P1_PIN(PIN0);\
																						Button_1_Total += T1TC-Button_1_Start;\
																						}  \
										 if (strcmp(pxCurrentTCB->pcTaskName, "BM2" ) == 0 )\
																						{\
																						CLR_P0_PIN(PIN2);\
																						Button_2_Total += T1TC-Button_2_Start;\
																						}  \
										 if (strcmp(pxCurrentTCB->pcTaskName, "PT" ) == 0 )\
																						{\
																						CLR_P0_PIN(PIN3);\
																						Periodic_Transmitter_Total += T1TC-Periodic_Transmitter_Start;\
																						}  \
										 if (strcmp(pxCurrentTCB->pcTaskName, "UR" ) == 0 )\
																						{\
																						CLR_P0_PIN(PIN4);\
																						Uart_Receiver_Total += T1TC-Uart_Receiver_Start;\
																						}  \
										 if (strcmp(pxCurrentTCB->pcTaskName, "LS1" ) == 0 )\
																						{\
																						CLR_P0_PIN(PIN5);\
																						Load_1_total += T1TC-Load_1_Start;\
																						}  \
										 if (strcmp(pxCurrentTCB->pcTaskName, "LS2" ) == 0 )\
																						{\
																						CLR_P0_PIN(PIN6);\
																						Load_2_total += T1TC-Load_2_Start;\
																						}  \
										 if (strcmp(pxCurrentTCB->pcTaskName, "IDLE" ) == 0 )\
																						{\
																						CLR_P0_PIN(PIN7);\
																						}  						\
										 System_Time = T1TC;\
										 Cpu_Load = (Button_1_Total + Button_2_Total + Periodic_Transmitter_Total + Uart_Receiver_Total + Load_1_total + Load_2_total ) /(float) System_Time * 100 ;\
										 
										 
#endif /* FREERTOS_CONFIG_H */