cmake_minimum_required(VERSION 3.1)

project(HttpClient)
add_executable(${PROJECT_NAME}
    "main.cpp"
    "httpclient.h"
    "httpclient.cpp"
    "httpclientprivate.h"
    "httpclientprivate.cpp")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_LIB
    "httpclient.h"
    "httpclient.cpp"
    "httpclientprivate.h"
    "httpclientprivate.cpp")

set ( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )
add_library( cpphttpclient SHARED ${SOURCE_LIB} )

if(WIN32)
  target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
  target_link_libraries(cpphttpclient wsock32 ws2_32)
endif()

