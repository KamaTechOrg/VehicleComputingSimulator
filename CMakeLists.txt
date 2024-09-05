cmake_minimum_required(VERSION 3.10)
project(DPCPPExample)

# הגדרת גרסת ה-C++
set(CMAKE_CXX_STANDARD 17)

# אופציה להוספת SYCL
option(USE_SYCL "Enable SYCL support" ON)

if(USE_SYCL)
    # הגדרת מהדר icpx עם תמיכה ב-SYCL
    set(CMAKE_CXX_COMPILER icpx)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsycl")

    # הוספת ספריות ההכללה של oneAPI
    include_directories(${CMAKE_SOURCE_DIR}/include /opt/intel/oneapi/compiler/latest/linux/include)
    # הוספת ספריות הקישור של oneAPI
    link_directories(/opt/intel/oneapi/compiler/latest/linux/lib)
endif()

# הוספת ספריות ההכללה והקישור של Google Test
include_directories(/usr/include /usr/local/include)
link_directories(/usr/lib /usr/local/lib)

# הוספת קבצי המקור
add_executable(DPCPPExample src/SHA256.cpp sha256_tests.cpp)

if(USE_SYCL)
    # קישור עם ספריית SYCL וספריות נוספות לפי הצורך
    target_link_libraries(DPCPPExample sycl)
else()
    # במקרה שאין שימוש ב-SYCL, לוודא קישור עם ספריות נוספות אם יש צורך
    # target_link_libraries(DPCPPExample)
endif()

# מציאת ספריית Google Test וקישור אליה
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
target_link_libraries(DPCPPExample ${GTEST_LIBRARIES} pthread)