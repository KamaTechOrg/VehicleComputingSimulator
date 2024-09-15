cmake_minimum_required(VERSION 3.10)
project(ImageProcessing)

set(CMAKE_CXX_STANDARD 14)

# adding libraries
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(include ../logger)

# make the library with the src files
file(GLOB SOURCES "src/*.cpp" "../logger/logger.cpp")
add_library(ImageProcessingLib ${SOURCES})

# create test executable
add_executable(runTests tests/main.cpp tests/test_serialize.cpp tests/test_detector.cpp tests/test_dynamic_tracker.cpp)
target_link_libraries(runTests ImageProcessingLib ${OpenCV_LIBS} ${GTEST_LIBRARIES} pthread)

# create main executable from src
add_executable(runMain src/main.cpp)
target_link_libraries(runMain ImageProcessingLib ${OpenCV_LIBS})

# adding tests
enable_testing()
add_test(NAME runTests COMMAND runTests)
