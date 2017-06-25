cmake_minimum_required(VERSION 3.4)
project(Minuit2 CXX)

# Inherit default from parent project if not main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

install(EXPORT Minuit2-targets DESTINATION Minuit2)

## Allow build directory to work for CMake import

export(TARGETS Math Minuit2 FILE Minuit2-exports.cmake)
export(PACKAGE Minuit2)

