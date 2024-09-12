cmake_minimum_required(VERSION 3.4)

# Project name
project(MyRSAProject CXX)

# Direct CMake to use icpx rather than the default C++ compiler/linker


# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()


# Find Google Test
find_package(GTest REQUIRED)

# Include GTest headers
include_directories(${GTEST_INCLUDE_DIRS})


# Options to choose between SYCL or Threads version with default set to Threads
option(USE_SYCL "Use SYCL version" OFF)
option(USE_THREADS "Use Threads version" ON)


# Ensure that if neither SYCL nor Threads is selected, it defaults to Threads
if(NOT USE_SYCL AND NOT USE_THREADS)
    set(USE_THREADS ON CACHE BOOL "Default to Threads version" FORCE)
endif()

# Set compiler based on the version chosen
if(USE_SYCL)
    message(STATUS "Using SYCL version")
    set(CMAKE_CXX_COMPILER icpx)  # Set the SYCL compiler
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsycl")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsycl")
    add_definitions(-SYCL_VERSION)
elseif(USE_THREADS)
    message(STATUS "Using Threads version")
endif()

# Set optimization level for Release build
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    # set(CMAKE_CXX_FLAGS_RELEASE "-O3 ")
else()
    # Set debugging flags for Debug build
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Sources for common files
set(COMMON_SOURCES
    src/big_int10.cpp
    src/rsa.cpp
    src/logger.cpp
)

# Determine which version to use: SYCL or Threads
if(USE_SYCL)
    list(APPEND COMMON_SOURCES
        src/sycl-version/big_int64.cpp
        src/sycl-version/prime_tests.cpp
    )
    include_directories(${CMAKE_SOURCE_DIR}/include/sycl-version)
elseif(USE_THREADS)
    list(APPEND COMMON_SOURCES
        src/threads-version/big_int64.cpp   
        src/threads-version/prime_tests.cpp
    )
    include_directories(${CMAKE_SOURCE_DIR}/include/threads-version)
endif()

# Print information about the chosen build type and version
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
if(USE_SYCL)
    message(STATUS "Version: SYCL")
elseif(USE_THREADS)
    message(STATUS "Version: Threads")
endif()


# Add unit tests
add_executable(TestBigInt10 tests/test_big_int10.cpp ${COMMON_SOURCES})
add_executable(TestBigInt64 tests/test_big_int64.cpp ${COMMON_SOURCES} tests/tests_utils.cpp)
add_executable(TestRSA tests/test_rsa.cpp ${COMMON_SOURCES} tests/tests_utils.cpp)

# Link GoogleTest with test executables
target_link_libraries(TestBigInt10 ${GTEST_LIBRARIES} pthread)
target_link_libraries(TestBigInt64 ${GTEST_LIBRARIES} pthread)
target_link_libraries(TestRSA ${GTEST_LIBRARIES} pthread)
# Enable testing
enable_testing()
# Add tests to CTest
add_test(NAME TestBigInt10 COMMAND TestBigInt10)
add_test(NAME TestBigInt64 COMMAND TestBigInt64)
add_test(NAME TestRSA COMMAND TestRSA)




