# All the source files in the project

set(APP_SOURCE_FILES
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_uart.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_uart.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_uart.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_uart.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_port.h
   ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.c
   ${CMAKE_CURRENT_SOURCE_DIR}/pin_mux.c
   ${CMAKE_CURRENT_SOURCE_DIR}/pin_mux.h
   ${CMAKE_CURRENT_SOURCE_DIR}/board.c
   ${CMAKE_CURRENT_SOURCE_DIR}/board.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_common.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_common.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/utilities/fsl_debug_console.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/utilities/fsl_debug_console.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/system_MKW24D5.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/system_MKW24D5.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_gpio.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_gpio.c
   ${CMAKE_CURRENT_SOURCE_DIR}/clock_config.c
   ${CMAKE_CURRENT_SOURCE_DIR}/clock_config.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_smc.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_smc.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_flash.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_flash.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_clock.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/drivers/fsl_clock.c
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
   CACHE INTERNAL ""
   )


 
