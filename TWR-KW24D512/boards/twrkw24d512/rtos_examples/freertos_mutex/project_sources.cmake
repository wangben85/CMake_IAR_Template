# All the source files in the project

set(APP_SOURCE_FILES
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_uart.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_uart.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_uart.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_uart.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_port.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_common.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_common.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/utilities/fsl_debug_console.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/utilities/fsl_debug_console.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/system_MKW24D5.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/system_MKW24D5.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_gpio.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_gpio.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_smc.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_smc.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_flash.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_flash.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_clock.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_clock.c

   # current project files
   ${CMAKE_CURRENT_SOURCE_DIR}/freertos_mutex.c
   ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOSConfig.h
   ${CMAKE_CURRENT_SOURCE_DIR}/pin_mux.c
   ${CMAKE_CURRENT_SOURCE_DIR}/pin_mux.h
   ${CMAKE_CURRENT_SOURCE_DIR}/board.c
   ${CMAKE_CURRENT_SOURCE_DIR}/board.h
   ${CMAKE_CURRENT_SOURCE_DIR}/clock_config.c
   ${CMAKE_CURRENT_SOURCE_DIR}/clock_config.h

   # FreeRTOS source files
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/croutine.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/event_groups.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/list.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/queue.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/tasks.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/timers.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/portable/IAR/ARM_CM3/fsl_tickless_systick.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/portable/IAR/ARM_CM3/port.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/portable/IAR/ARM_CM3/fsl_tickless_lptmr.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/portable/MemMang/heap_4.c
   CACHE INTERNAL ""
   )

set(APP_INCLUDE_DIRS
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../CMSIS/Include
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/utilities
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5
   ${CMAKE_CURRENT_SOURCE_DIR}/../iar
   ${CMAKE_CURRENT_SOURCE_DIR}/../
   ${CMAKE_CURRENT_SOURCE_DIR}

   #FreeRTOS header files
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/portable/IAR/ARM_CM3
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/include
   CACHE INTERNAL ""
   )


 
