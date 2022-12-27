cmake_minimum_required (VERSION 3.2)

project ("VIDEO_K")

SET(CMAKE_SYSTEM_NAME Linux)
# SET(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
# SET(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")


message(STATUS "build cmake ----------------------------------------------------------")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


# include
# include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${SSD202D_PROJECT_PATH}/release/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/v4l2/inc)
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/v4l2/src)
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/v4l2/inc/internal)
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/x264/inc)


# lib

link_directories(${PROJECT_SOURCE_DIR}/lib)
# link_directories(${PROJECT_SOURCE_DIR}/lib/v4l2)
link_directories(${PROJECT_SOURCE_DIR}/lib/x264/lib)




# src
aux_source_directory (${CMAKE_CURRENT_LIST_DIR}/src	SRC_CPP)
# aux_source_directory (${CMAKE_CURRENT_LIST_DIR}/lib/v4l2/src SRC_CPP)


add_executable (${PROJECT_NAME} ${SRC_CPP})

# target_link_libraries(${PROJECT_NAME} m rt mi_vdec mi_sys mi_disp mi_panel dl)



message(STATUS "build cmake ----------------------------------------------------------")
#cmake中执行shell指令
execute_process(COMMAND pwd)