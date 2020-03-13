cmake_minimum_required(VERSION 3.0)

project("freertos_event")

# Default to Debug if no build type specified
if (NOT CMAKE_BUILD_TYPE)
   message("No build type specified, defaulting to Debug")
   set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

#-------------------------------------------------------------------------------
# Sources and includes defined in one large file parsed from IAR project file
# to get build up and running as quickly as possible.
#-------------------------------------------------------------------------------
include(project_sources.cmake)
set(SOURCE_FILES ${APP_SOURCE_FILES})
set(INCLUDE_DIRS
   ${APP_INCLUDE_DIRS}
   )

#-------------------------------------------------------------------------------
# Build library from assembly sources. This is necessary so that we can define
# compile flags and include directories just for assembly sources.
#-------------------------------------------------------------------------------

enable_language(ASM)

set(ASM_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../devices/MKW24D5/iar/startup_MKW24D5.S
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../../rtos/freertos_9.0.0/Source/portable/IAR/ARM_CM3/portasm.s
   )
set(ASM_INCLUDE_DIRS
   ${CMAKE_CURRENT_SOURCE_DIR}/
   )
set(ASM_COMPILE_FLAGS
   -s+
   -M<>
   -w+
   --cpu Cortex-M4
   --fpu None
   )

add_library(asm OBJECT ${ASM_SOURCES})
target_compile_options(asm PRIVATE ${ASM_COMPILE_FLAGS})
target_include_directories(asm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)   # asm source include header
#-------------------------------------------------------------------------------
# Define main project binary
#-------------------------------------------------------------------------------

set(COMMON_COMPILER_FLAGS

   # Define DNDEBUG for release builds
   $<$<CONFIG:Release>:-DNDEBUG>

   # Proprocessor defines from IAR project file
   -DCPU_MKW24D512VHA5
   -DIAR
   -DFSL_RTOS_FREE_RTOS
   -DPRINTF_FLOAT_ENABLE=0
   -DSCANF_FLOAT_ENABLE=0
   -DPRINTF_ADVANCED_ENABLE=0
   -DSCANF_ADVANCED_ENABLE=0
   -DTWR_KW24D512
   -DTOWER

   # Suppress warnings
   # Pa050 non-native line endings
   # Pa039 using address of unaligned structure
   --diag_suppress Pa082,Pa050,Pe167,Pa182

   # Default endianess is little, but no harm in forcing it
   # Specify cpu so compiler knows what its compiling for
   # Cortex-M4 can support fpu, but we don't have one so need software impl
   --endian=little
   --cpu=Cortex-M4
   --fpu=None

   # Save code space and screen space by not use full
   # filepaths in __FILE__ macros
   --no_path_in_file_macros

   # Keep output brief unless error
   --silent

   # Generate list files
   -lC "."

   # Allow IAR langugage extensions
   -e

   # We're using the normal config of DLIB
   --dlib_config "${IAR_TOOLCHAIN_PATH}/arm/inc/c/DLib_Config_Normal.h"

   # C++ specific options. Disallowing exceptions, rtti, and
   # destruction of static objects saves code space
   $<$<COMPILE_LANGUAGE:CXX>:--no_exceptions>
   $<$<COMPILE_LANGUAGE:CXX>:--no_rtti>
   $<$<COMPILE_LANGUAGE:CXX>:--no_static_destruction>
   )

set(CMAKE_BUILD_TYPES "Debug;Release" CACHE STRING "" FORCE)

#-------------------------------------------------------------------------------
# Define main app binary
#-------------------------------------------------------------------------------

add_executable(${PROJECT_NAME}
   ${SOURCE_FILES}
   $<TARGET_OBJECTS:asm>
   )
target_compile_options(${PROJECT_NAME}
   PRIVATE
   ${COMMON_COMPILER_FLAGS}
   $<$<CONFIG:Release>:
   -Oh>  # change optimization from -Oh(High Balanced) to -Ohz(High favoring size)
   $<$<CONFIG:Debug>:
   -Om
   --debug
   --no_cse
   --no_unroll
   --no_inline
   --no_code_motion
   --no_tbaa
   --no_clustering
   --no_scheduling>
   )
target_include_directories(${PROJECT_NAME}
   PRIVATE
   ${INCLUDE_DIRS}
   )

# Specify linker flags
set_target_properties(${PROJECT_NAME}
   PROPERTIES
   LINK_FLAGS
   "--config_def=gUseBootloaderLink_d=1 \
   --config_def=APPLICATION_BUILD=1 \
   --config \"${CMAKE_CURRENT_SOURCE_DIR}/iar/MKW24D512xxx5_flash.icf\" \
   --redirect _Printf=_PrintfLargeNoMb \
   --redirect _Scanf=_ScanfFullNoMb \
   --map \"${CMAKE_CURRENT_SOURCE_DIR}/iar/${CMAKE_BUILD_TYPE}/List/${PROJECT_NAME}.map\" \
   --entry __iar_program_start \
   --inline \
   --no_exceptions \
   --vfe=forced \
   --merge_duplicate_sections"
   SUFFIX
   .out)

# Output binary in location consistent with IAR's IDE default placement.
set_target_properties(${PROJECT_NAME}
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY
   ${CMAKE_CURRENT_SOURCE_DIR}/iar/${CMAKE_BUILD_TYPE}/Exe/
   )

# Release binaries have a CRC16 checksum appeneded, which is used by firmware
# update process. If we're building a release binary, execute tools to generate
# such a binary image.

#if ( ${CMAKE_BUILD_TYPE} MATCHES Release )   #only build bin in release build
   add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${IAR_TOOLCHAIN_PATH}/arm/bin/ielftool.exe
      ${CMAKE_CURRENT_SOURCE_DIR}/iar/${CMAKE_BUILD_TYPE}/Exe/${CMAKE_PROJECT_NAME}.out
      --bin
      ${CMAKE_CURRENT_SOURCE_DIR}/iar/${CMAKE_BUILD_TYPE}/Exe/${CMAKE_PROJECT_NAME}.bin
      )
#endif()
