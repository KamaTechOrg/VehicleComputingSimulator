# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
# שם הפרויקט
project(WebSocketApp)
# הגדרת תקן C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# מציאת חבילת Boost הנדרשת
find_package(Boost 1.70 REQUIRED COMPONENTS system thread)
# הגדרת include directories עבור Boost
include_directories(${Boost_INCLUDE_DIRS})
# הוספת קובץ המקור של השרת
add_executable(websocket_server websocket_server.cpp)
# קישור Boost לשרת
target_link_libraries(websocket_server ${Boost_LIBRARIES} pthread)
# הוספת קובץ המקור של הלקוח
add_executable(websocket_client websocket_client.cpp)
# קישור Boost ללקוח
target_link_libraries(websocket_client ${Boost_LIBRARIES} pthread)