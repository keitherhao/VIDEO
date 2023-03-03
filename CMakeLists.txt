cmake_minimum_required (VERSION 3.2)

project ("VIDEO_K")

set(CMAKE_SYSTEM_NAME Linux)

# if(ARM32)
#     SET(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
#     SET(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
#     message(STATUS "build cmake ---------------------------------------------------------- ARM32:"${ARM32})
# endif(ARM32)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# include
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/linux/tools/testing/selftests/rcutorture/formal/srcu-cbmc/include/linux)
include_directories(${CMAKE_CURRENT_LIST_DIR}/linux/include/uapi/linux)
include_directories(${CMAKE_CURRENT_LIST_DIR}/linux/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/linux/arch/arm64/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/FFmpeg)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/x264)

# lib
link_directories(/usr/local/lib)


# src
aux_source_directory (${CMAKE_CURRENT_LIST_DIR}/src SRC_CPP)


add_executable(${PROJECT_NAME}   ${SRC_CPP})
target_link_libraries(${PROJECT_NAME} avcodec avutil swresample)
target_link_libraries(${PROJECT_NAME} x264)
target_link_libraries(${PROJECT_NAME} x265)
target_link_libraries(${PROJECT_NAME} pthread m z)

# cmake中执行shell指令
execute_process(COMMAND pwd)
