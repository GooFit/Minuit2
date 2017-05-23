cmake_minimum_required(VERSION 3.4)
project(Minuit2 CXX)

# Inherit default from parent project if not main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# Buggy, do not activate
# find_package(OpenMP)
# if(OpenMP_FOUND)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     find_package(Threads REQUIRED)
#     link_libraries(Threads::Threads)
#     link_libraries(${OpenMP_CXX_FLAGS})
# endif()

add_subdirectory(src)

# Only add tests if this is the main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(test)
endif()

