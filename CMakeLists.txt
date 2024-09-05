
cmake_minimum_required(VERSION 3.10)
project(DPCPPExample)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Option to enable SYCL support
option(USE_SYCL "Enable SYCL support" ON)

if(USE_SYCL)
    # Set the compiler to icpx with SYCL support
    set(CMAKE_CXX_COMPILER icpx)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsycl")

    # Add oneAPI include directories
    include_directories(${CMAKE_SOURCE_DIR}/include /opt/intel/oneapi/compiler/latest/linux/include)
    # Add oneAPI library directories
    link_directories(/opt/intel/oneapi/compiler/latest/linux/lib)
endif()

# Add Google Test include and library directories
include_directories(/usr/include /usr/local/include)
link_directories(/usr/lib /usr/local/lib)

# Add source files
add_executable(DPCPPExample src/sha256.cpp tests/sha256_tests.cpp)

if(USE_SYCL)
    # Link with SYCL library and any additional libraries as needed
    target_link_libraries(DPCPPExample sycl)
else()
    # If not using SYCL, make sure to link with any additional libraries if needed
    # target_link_libraries(DPCPPExample)
endif()

# Find and link Google Test library
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
target_link_libraries(DPCPPExample ${GTEST_LIBRARIES} pthread)
