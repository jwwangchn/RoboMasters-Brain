cmake_minimum_required(VERSION 2.8)

project(robomasters-brain)

set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE} -std=Release")

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

include_directories(asp/inc)
include_directories(uart/linux) 
include_directories(fastcluster)
include_directories(logging)

FILE(GLOB sourcefiles "asp/src/*.c" "uart/linux/*.cpp" "fastcluster/*.cpp"  "*.cpp" "*.c")

add_executable(robomasters-brain ${sourcefiles})

# library
set(DEPENDENCIES realsense)
find_package(OpenCV 2 REQUIRED)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")

target_link_libraries(robomasters-brain pthread ${OpenCV_LIBS} ${DEPENDENCIES})