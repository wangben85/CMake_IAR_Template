/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "board.h"
#include "fsl_port.h"
#include "pin_mux.h"
#include "fsl_common.h"
#include "clock_config.h"
#include "fsl_flash.h"
#include "fsl_smc.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_LED_GPIO BOARD_LED_BLUE1_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_BLUE1_GPIO_PIN

#define BOARD_SW_GPIO BOARD_SW3_GPIO
#define BOARD_SW_PORT BOARD_SW3_PORT
#define BOARD_SW_GPIO_PIN BOARD_SW3_GPIO_PIN
#define BOARD_SW_IRQ BOARD_SW3_IRQ
#define BOARD_SW_IRQ_HANDLER BOARD_SW3_IRQ_HANDLER
#define BOARD_SW_NAME BOARD_SW3_NAME

/* Task priorities. */
/* clang-format off */
#define start_task_PRIORITY   ( configMAX_PRIORITIES - 2 )
#define led_task_PRIORITY   ( configMAX_PRIORITIES - 2 )
#define getinfo_task_PRIORITY   ( configMAX_PRIORITIES - 2 )
#define SW_task_PRIORITY   ( configMAX_PRIORITIES - 1 )
#define TIME_DELAY_SLEEP      1000

/* Interrupt priorities. */
#define SW_NVIC_PRIO 2

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
TaskHandle_t StartTask_Handler;
TaskHandle_t LED0Task_Handler;
TaskHandle_t QUERYTask_Handler;
/*******************************************************************************
 * Variables
 ******************************************************************************/
static void start_task(void *pvParameters);
static void led0_task(void *pvParameters);
static void SW_task(void *pvParameters);
static void query_task(void *pvParameters);

SemaphoreHandle_t xSWSemaphore = NULL;

char InfoBuffer[1000]; //vTasklist
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
/* Define the init structure for the input switch pin */
#ifdef BOARD_SW_NAME
    gpio_pin_config_t sw_config = {
        kGPIO_DigitalInput, 0,
    };
#endif

/* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, 0,
    };

    BOARD_InitPins();
    BOARD_BootClockRUN();//inside BOARD_BootClockRUN, enable Fast IRC in normal mode and stop mode
    BOARD_InitDebugConsole();

    /* Print a note to terminal. */
    PRINTF("Freertos_taskgetinfo Demo example\r\n");

#ifdef BOARD_SW_NAME
    PRINTF("Press %s to wake up the CPU\r\n", BOARD_SW_NAME);
    /* Init input switch GPIO. */
    PORT_SetPinInterruptConfig(BOARD_SW_PORT, BOARD_SW_GPIO_PIN, kPORT_InterruptFallingEdge);
    NVIC_SetPriority(BOARD_SW_IRQ, SW_NVIC_PRIO);
    EnableIRQ(BOARD_SW_IRQ);
    GPIO_PinInit(BOARD_SW_GPIO, BOARD_SW_GPIO_PIN, &sw_config);
#endif

    /* LED Init */
    GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_GPIO_PIN, &led_config);
    
    /*Create tickless task*/
    xTaskCreate(start_task, "start_task", configMINIMAL_STACK_SIZE + 38, NULL, start_task_PRIORITY, &StartTask_Handler);//start task, priority = 
    xTaskCreate(SW_task, "SW_task", configMINIMAL_STACK_SIZE + 38, NULL, SW_task_PRIORITY, NULL); //sw task, priority 
    /*Task Scheduler*/
    vTaskStartScheduler();
    for (;;)
        ;
}

/* Start Task */
void start_task(void *pvParameters)
{
    //led0 task
    xTaskCreate(led0_task,     	
                "led0_task",   	
                configMINIMAL_STACK_SIZE + 38, 
                NULL,				
                start_task_PRIORITY,	
                &LED0Task_Handler);   
    //getInfo task
    xTaskCreate(query_task,    
                "query_task",  
                configMINIMAL_STACK_SIZE + 38, 
                NULL,
                getinfo_task_PRIORITY,	
                &QUERYTask_Handler);       
  
    vTaskDelete(StartTask_Handler);
}

/* LED0 Task */
static void led0_task(void *pvParameters)
{
    PRINTF("\r\nLED0 Task :\r\n");
    for (;;)
    {
        /*PRINTF("\r%d\r\n", xTaskGetTickCount());*/
        PRINTF("\r\nLED0 Toggle every 1 second :\r\n");
        LED_BLUE1_TOGGLE();
        vTaskDelay(TIME_DELAY_SLEEP); // time delay time = 1s here, led0 task will release CPU time to other tasks for 1 seconds
                                      // LED0 toggle
    }
}

/* Switch Task */
static void SW_task(void *pvParameters)
{
    xSWSemaphore = xSemaphoreCreateBinary();
    for (;;)
    {
        if (xSemaphoreTake(xSWSemaphore, portMAX_DELAY) == pdTRUE) // blocking wait for the semaphore
        {
            PRINTF("CPU waked up by EXT interrupt\r\n");
        }
    }
}

static void query_task(void *pvParameters)
{
    for(;;) 
    {
      	TaskHandle_t TestHandle;
	
	uint32_t TotalRunTime;
	UBaseType_t Priority;
	UBaseType_t ArraySize,x;
	TaskStatus_t *StatusArray;
	
	UBaseType_t  StackMinSize;
	
	eTaskState TaskState;
	
        ArraySize = uxTaskGetNumberOfTasks();	
        StatusArray = pvPortMalloc(ArraySize * sizeof(TaskStatus_t));
        if(StatusArray != NULL)  
        {
         // get all the tasks state
           ArraySize = uxTaskGetSystemState((TaskStatus_t*	)StatusArray, 
                                            (UBaseType_t	)ArraySize, 
                                            (uint32_t *	)&TotalRunTime );
           printf("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
           for(x=0;x<ArraySize;x++)
           {
            PRINTF("%10s\t\t%d\t\t\t%d\t\t\t\r\n",		  // %10s for format the task name not alignment		
                     StatusArray[x].pcTaskName,
                     (int)StatusArray[x].uxCurrentPriority,
                     (int)StatusArray[x].xTaskNumber);
                }
        }
        vPortFree(StatusArray);
        
#if 0   
        // get tasks infomration from vTaskList
        vTaskList(InfoBuffer);
        printf("\t\t\tState\tPri\tStack\tNo\t\t\r\n");
        PRINTF("%s\r\n",InfoBuffer);
#endif

        vTaskDelay(5 * TIME_DELAY_SLEEP); // print task infomation every 5 seconds
    }
}
/*!
 * @brief Interrupt service fuction of switch.
 *
 * This function to wake up CPU
 */
#ifdef BOARD_SW_NAME
void BOARD_SW_IRQ_HANDLER(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* Clear external interrupt flag. */
    GPIO_ClearPinsInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);

    xSemaphoreGiveFromISR(xSWSemaphore, &xHigherPriorityTaskWoken); //SW IRQ give the semaphore to SW task
}
#endif
