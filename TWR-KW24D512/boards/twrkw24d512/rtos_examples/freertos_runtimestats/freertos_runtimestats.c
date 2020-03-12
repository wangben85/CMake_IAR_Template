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
#include "fsl_lptmr.h"
#include "freertos_runtimestats.h"
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
#define start_task_PRIORITY           ( configMAX_PRIORITIES - 2 )
#define led_task_PRIORITY             ( configMAX_PRIORITIES - 2 )
#define getinfo_task_PRIORITY         ( configMAX_PRIORITIES - 2 )
#define runtimestats_task_PRIORITY    ( configMAX_PRIORITIES - 1 ) //priority is the highest
#define TIME_DELAY_SLEEP              1000

/* Interrupt priorities. */
#define SW_NVIC_PRIO 2

/* lptmr */
#define LPTMR_LED_HANDLER LPTMR0_IRQHandler
/* Get source clock for LPTMR driver */
#define LPTMR_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_McgInternalRefClk) //LPTMR use the Fast IRC Clock = 4Mhz
/* Define LPTMR microseconds counts value */
#define LPTMR_USEC_COUNT 50           // set lptmr periodic = 50us, which is 20 times of the system tick period(1000us)

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
static void runtimestats_task(void *pvParameters);
static void query_task(void *pvParameters);

void LPTMR_Module_Init();

SemaphoreHandle_t xSWSemaphore = NULL;

char TaskInfoBuffer[1000]; //vTasklist
char TaskRunTimeStatsBuffer[1000]; //vTaskGetRunTimeStats

volatile uint32_t lptmrTicks = 0U; // Note!!!, please don't use 'static' here

void ConfigureTimerForRunTimeStats(void)
{
   lptmrTicks = 0;
   LPTMR_Module_Init(); 
}

void LPTMR_Module_Init()
{
    lptmr_config_t lptmrConfig;
    /* Configure LPTMR */
    /*
     * lptmrConfig.timerMode = kLPTMR_TimerModeTimeCounter;
     * lptmrConfig.pinSelect = kLPTMR_PinSelectInput_0;
     * lptmrConfig.pinPolarity = kLPTMR_PinPolarityActiveHigh;
     * lptmrConfig.enableFreeRunning = false;
     * lptmrConfig.bypassPrescaler = true;
     * lptmrConfig.prescalerClockSource = kLPTMR_PrescalerClock_1;
     * lptmrConfig.value = kLPTMR_Prescale_Glitch_0;
     */
    LPTMR_GetDefaultConfig(&lptmrConfig);
    lptmrConfig.prescalerClockSource = kLPTMR_PrescalerClock_0; //Clock source Fast MCRIRCLK = 4Mhz;
    lptmrConfig.bypassPrescaler = false; //don't bypass prescaler
    lptmrConfig.value = kLPTMR_Prescale_Glitch_1; //Prescaler divides the prescaler clock by 4, 4Mhz / 4 = 1Mhz

    /* Initialize the LPTMR */
    LPTMR_Init(LPTMR0, &lptmrConfig);

    /*
     * Set timer period.
     * Note : the parameter "ticks" of LPTMR_SetTimerPeriod should be equal or greater than 1.
    */
    LPTMR_SetTimerPeriod(LPTMR0, USEC_TO_COUNT(LPTMR_USEC_COUNT, LPTMR_SOURCE_CLOCK)); //period = 50us

    /* Enable timer interrupt */
    LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable);

    /* Enable at the NVIC */
    EnableIRQ(LPTMR0_IRQn);

    /* Start counting */
    LPTMR_StartTimer(LPTMR0);
}

/* LPTMR interrupt routinue */
void LPTMR_LED_HANDLER(void)
{
    LPTMR_ClearStatusFlags(LPTMR0, kLPTMR_TimerCompareFlag);
    lptmrTicks++;
}
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
    PRINTF("Freertos_runtimestats Demo example\r\n");

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
    xTaskCreate(start_task, "start_task", configMINIMAL_STACK_SIZE + 38, NULL, start_task_PRIORITY, &StartTask_Handler);//start task, 
    xTaskCreate(runtimestats_task, "runtimestats_task", configMINIMAL_STACK_SIZE + 38 , NULL, runtimestats_task_PRIORITY, NULL); //runtimestats task,  allocate more stack size for this task
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
        PRINTF("\r\nLED0 Toggle every 1 second :\r\n");
        LED_BLUE1_TOGGLE();
        vTaskDelay(TIME_DELAY_SLEEP ); // time delay time = 1s here, led0 task will release CPU time to other tasks for 1 seconds
                                      // LED0 toggle
    }
}

/* Get Runtimestats Task */
static void runtimestats_task(void *pvParameters)
{
    xSWSemaphore = xSemaphoreCreateBinary();
    for (;;)
    {
        if (xSemaphoreTake(xSWSemaphore, portMAX_DELAY) == pdTRUE) // blocking wait for the semaphore
        {
            PRINTF("CPU waked up by EXT interrupt\r\n");
            vTaskGetRunTimeStats(TaskRunTimeStatsBuffer);
            PRINTF("%s\r\n",TaskRunTimeStatsBuffer);
        }
    }
}

static void query_task(void *pvParameters)
{
    for(;;) 
    {
#if 0       
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
           PRINTF("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
           for(x=0;x<ArraySize;x++)
           {
            PRINTF("%10s\t\t%d\t\t\t%d\t\t\t\r\n",		  // %10s for format the task name not alignment		
                     StatusArray[x].pcTaskName,
                     (int)StatusArray[x].uxCurrentPriority,
                     (int)StatusArray[x].xTaskNumber);
                }
        }
        vPortFree(StatusArray);
#endif        

#if 0 
        // get tasks infomration from vTaskList
        vTaskList(TaskInfoBuffer);
        PRINTF("\t\t\tState\tPri\tStack\tNo\t\t\r\n");
        PRINTF("%s\r\n",TaskInfoBuffer);
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
