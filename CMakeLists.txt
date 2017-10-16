cmake_minimum_required(VERSION 3.4)
project(Minuit2 CXX)

# Inherit default from parent project if not main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(MinuitCommon INTERFACE)

# Buggy, do not activate
# find_package(OpenMP)
# if(OpenMP_FOUND)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     find_package(Threads REQUIRED)
#     link_libraries(Threads::Threads)
#     link_libraries(${OpenMP_CXX_FLAGS})
# endif()

# Uses the old CXX bindings (deprecated), probably do not activate
option(ENABLE_MPI "Enable MPI")
if(ENABLE_MPI)
    find_package(MPI REQUIRED)

    # For supporting CMake < 3.9:
    if(NOT TARGET MPI::MPI_CXX)
        add_library(MPI_LIB_TARGET INTERFACE)
        add_library(MPI::MPI_CXX ALIAS MPI_LIB_TARGET)

        target_compile_options(MPI_LIB_TARGET INTERFACE "${MPI_CXX_COMPILE_FLAGS}")
        target_include_directories(MPI_LIB_TARGET INTERFACE "${MPI_CXX_INCLUDE_PATH}")
        target_link_libraries(MPI_LIB_TARGET INTERFACE ${MPI_CXX_LINK_FLAGS} ${MPI_CXX_LIBRARIES})
    endif()

    message(STATUS "Building Minuit2 with MPI support")
    message("Run: ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS} EXECUTABLE ${MPIEXEC_POSTFLAGS} ARGS")
    target_compile_definitions(MinuitCommon INTERFACE "-DMPIPROC")
    target_link_libraries(MinuitCommon INTERFACE MPI::MPI_CXXX)
endif()

add_subdirectory(src)

install(DIRECTORY inc/Fit DESTINATION include/Minuit2)
install(DIRECTORY inc/Math DESTINATION include/Minuit2)
install(DIRECTORY inc/Minuit2 DESTINATION include/Minuit2)

# Only add tests if this is the main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(test)
endif()

install(EXPORT Minuit2Config DESTINATION lib/cmake/Minuit2)

## Allow build directory to work for CMake import
export(TARGETS MinuitCommon Math Minuit2 FILE Minuit2Targets.cmake)
export(PACKAGE Minuit2)

