cmake_minimum_required(VERSION 3.1)
project(Minuit2
    VERSION 6.10.08
    LANGUAGES CXX)

# Inherit default from parent project if not main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

# Common features to all packages (Math and Minuit2)
add_library(MinuitCommon INTERFACE)

# OpenMP support
option(MINUIT2_OMP "Enable OpenMP for Minuit2 (requires thread safe FCN function)")
if(MINUIT2_OMP)
    find_package(OpenMP REQUIRED)

    # For CMake < 3.9, we need to make the target ourselves
    if(NOT TARGET OpenMP::OpenMP_CXX)
        add_library(OpenMP_TARGET INTERFACE)
        add_library(OpenMP::OpenMP_CXX ALIAS OpenMP_TARGET)
        target_compile_options(OpenMP_TARGET INTERFACE ${OpenMP_CXX_FLAGS})
        find_package(Threads REQUIRED)
        target_link_libraries(OpenMP_TARGET INTERFACE Threads::Threads)
        target_link_libraries(OpenMP_TARGET INTERFACE ${OpenMP_CXX_FLAGS})
    endif()
    target_link_libraries(MinuitCommon INTERFACE OpenMP::OpenMP_CXX)
    message(STATUS "Building Minuit2 with OpenMP support")
endif()

# MPI support
# Uses the old CXX bindings (deprecated), probably do not activate
option(MINUIT2_MPI "Enable MPI for Minuit2")

if(MINUIT2_MPI)
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

install(EXPORT Minuit2Config DESTINATION share/cmake/Modules/Minuit2)

## Allow build directory to work for CMake import
export(TARGETS MinuitCommon Math Minuit2 FILE Minuit2Targets.cmake)
export(PACKAGE Minuit2)


# Packaging support
set(CPACK_PACKAGE_VENDOR "root.cern.ch")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Minuit2 standalone fitting tool")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
# CPack collects *everything* except what's listed here.
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /dist
    /.*build.*
    /\\\\.DS_Store
    /.*\\\\.egg-info
    /var
    /Pipfile.*$
)
include(CPack)

