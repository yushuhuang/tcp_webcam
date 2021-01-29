cmake_minimum_required(VERSION 3.5)

project(udp_opencv)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(OpenCV REQUIRED)

if(OPENCV_FOUND)
    message("Opencv Version: ${OPENCV_VERSION}" )
endif()

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(client client.cpp)
add_executable(server server.cpp)

target_link_libraries(client ${OpenCV_LIBS})
target_link_libraries(server ${OpenCV_LIBS})