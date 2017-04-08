cmake_minimum_required(VERSION 3.4)
project(Minuit2 CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenMP)
if(OpenMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    find_package(Threads REQUIRED)
    link_libraries(Threads::Threads)
endif()

add_subdirectory(src)
add_subdirectory(test)

