cmake_minimum_required(VERSION 3.10)

# שם הפרויקט
project(BasicApi)

# הגדרת התקנים
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# הוספת ספריות וכולל
include_directories(${CMAKE_SOURCE_DIR})

# הוספת קבצי מקור
set(COMMON_SOURCES
    communication.cpp
    myClass.cpp
)

# יצירת קובץ ביצוע עבור user1
add_executable(user1 user1.cpp ${COMMON_SOURCES})

# יצירת קובץ ביצוע עבור user2
add_executable(user2 user2.cpp ${COMMON_SOURCES})

# הוספת קישור לספריית pthread
target_link_libraries(user1 pthread)
target_link_libraries(user2 pthread)
