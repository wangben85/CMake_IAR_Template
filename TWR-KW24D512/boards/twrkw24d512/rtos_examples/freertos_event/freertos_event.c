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

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_common.h"
#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define BIT0 (1 << 0)
#define BIT0_DELAY_TIME   1000  //1000ms
#define BIT23 (1 << 23 )   // configUSE_16_BIT_TICKS = 0, totally 24 bit events
#define BIT23_DELAY_TIME   5000  //5000ms

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void write_task_1(void *pvParameters);
static void write_task_2(void *pvParameters);
static void read_task(void *pvParameters);
/*******************************************************************************
 * Globals
 ******************************************************************************/
static EventGroupHandle_t event_group = NULL;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    event_group = xEventGroupCreate();
    xTaskCreate(write_task_1, "WRITE_TASK_1", configMINIMAL_STACK_SIZE + 38, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(write_task_2, "WRITE_TASK_2", configMINIMAL_STACK_SIZE + 38, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(read_task, "READ_TASK", configMINIMAL_STACK_SIZE + 38, NULL, tskIDLE_PRIORITY + 2, NULL);
    /* Start scheduling. */
    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief write_task_1 function
 */
static void write_task_1(void *pvParameters)
{
    while (1)
    {
        xEventGroupSetBits(event_group, BIT0);
        vTaskDelay(BIT0_DELAY_TIME);
    }
}

/*!
 * @brief write_task_2 function
 */
static void write_task_2(void *pvParameters)
{
    while (1)
    {
        xEventGroupSetBits(event_group, BIT23);
        vTaskDelay(BIT23_DELAY_TIME);
    }
}

/*!
 * @brief read_task function
 */
static void read_task(void *pvParameters)
{
    EventBits_t event_bits;
    while (1)
    {
        event_bits = xEventGroupWaitBits(event_group,    /* The event group handle. */
                                         BIT0 | BIT23,        /* The bit pattern the event group is waiting for. */
                                         pdTRUE,         /* BIT_0 and BIT_4 will be cleared automatically. */
                                         pdFALSE,        /* Don't wait for both bits, either bit unblock task. */
                                         portMAX_DELAY); /* Block indefinitely to wait for the condition to be met. */

        if ((event_bits & (BIT0 | BIT23)) == (BIT0 | BIT23))
        {
            PRINTF("Both bits are set.");
        }
        else if ((event_bits & BIT0) == BIT0)
        {
            PRINTF("Bit 0 is set.\r\n");
        }
        else if ((event_bits & BIT23) == BIT23)
        {
            PRINTF("Bit 24 is set.\r\n");
        }
    }
}
