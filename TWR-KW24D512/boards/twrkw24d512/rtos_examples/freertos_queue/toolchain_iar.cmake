# Specify the IAR compiler

set(CMAKE_SYSTEM_NAME Generic)

set(IAR_TOOLCHAIN_PATH
   "C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.1"
   CACHE PATH
   "Toolchain path")

set(CMAKE_C_COMPILER   "${IAR_TOOLCHAIN_PATH}/arm/bin/iccarm.exe")
set(CMAKE_CXX_COMPILER "${IAR_TOOLCHAIN_PATH}/arm/bin/iccarm.exe" "--c++")
set(CMAKE_ASM_COMPILER "${IAR_TOOLCHAIN_PATH}/arm/bin/iasmarm.exe")
set(CMAKE_ASM_CREATE_STATIC_LIBRARY "<CMAKE_AR> <TARGET> --create <LINK_FLAGS> <OBJECTS> ")
