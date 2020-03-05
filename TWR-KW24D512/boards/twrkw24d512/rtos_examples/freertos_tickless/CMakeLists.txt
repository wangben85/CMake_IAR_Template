cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0058 NEW)

# Building regular app binary

if ( NOT IAR_TOOLCHAIN_PATH )
   message( FATAL_ERROR "Building regular app binary requires toolchain file "
         "be specified on the command line! e.g. -DCMAKE_TOOLCHAIN_FILE=../toolchain_iar"
         "\n"
         "To build unit tests, enable unit_test option. -DUNIT_TESTS=ON"
         )
endif()

    include(${CMAKE_CURRENT_SOURCE_DIR}/freertos_tickless.cmake)
