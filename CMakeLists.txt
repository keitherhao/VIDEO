cmake_minimum_required (VERSION 3.2)

project ("VIDEO_K")

SET(CMAKE_SYSTEM_NAME Linux)

if(ARM32)
    SET(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
    SET(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
    message(STATUS "build cmake ---------------------------------------------------------- ARM32:"${ARM32})
endif(ARM32)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# include
include_directories (${CMAKE_CURRENT_LIST_DIR}/include)
include_directories (${CMAKE_CURRENT_LIST_DIR}/linux/tools/testing/selftests/rcutorture/formal/srcu-cbmc/include/linux)
include_directories (${CMAKE_CURRENT_LIST_DIR}/linux/include/uapi/linux)
include_directories (${CMAKE_CURRENT_LIST_DIR}/linux/include)
include_directories (${CMAKE_CURRENT_LIST_DIR}/linux/arch/arm64/include)
# include_directories (${CMAKE_CURRENT_LIST_DIR}/linux/include/asm-generic)
#include_directories (${CMAKE_CURRENT_LIST_DIR}/../linux-5.19.3/include)

# src
aux_source_directory (${CMAKE_CURRENT_LIST_DIR}/src SRC_CPP)


add_executable (${PROJECT_NAME}   ${SRC_CPP})
#cmake中执行shell指令
execute_process(COMMAND pwd)
