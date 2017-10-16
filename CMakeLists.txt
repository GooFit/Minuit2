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

set(ENABLE_MPI FALSE CACHE BOOL "Enable MPI")

if (ENABLE_MPI)
   find_package(MPI)
   if (MPI_CXX_FOUND)
      # Set compilation flags
      if(${MPI_CXX_COMPILE_FLAGS})
         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS}")
      endif()
      include_directories(${MPI_CXX_INCLUDE_PATH})
      add_definitions("-DMPIPROC")
   endif()
endif()

add_subdirectory(src)

#Install the header files
install(DIRECTORY inc/ DESTINATION include)
   
install(EXPORT MINUIT2Config DESTINATION cmake)

# Only add tests if this is the main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(test)
endif()

