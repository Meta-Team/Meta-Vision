cmake_minimum_required(VERSION 3.0)
project(armorDetector)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
set(CMAKE_CXX_STANDARD 11)
set(ARMORDETECTOR
        main.cpp
        myHeader.hpp)

add_executable(armorDetector ${ARMORDETECTOR})

FIND_PACKAGE(OpenCV REQUIRED)
IF (OpenCV_FOUND)
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES(armorDetector ${OpenCV_LIBS})
ELSE (OpenCV_FOUND)
    MESSAGE(FATAL_ERROR "OpenCV library not found")
ENDIF (OpenCV_FOUND)
