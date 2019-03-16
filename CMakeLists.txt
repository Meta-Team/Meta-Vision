cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS_DEBUG "-Wall -g" CACHE STRING "")       
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE "-Wall -O3" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3" CACHE STRING "")
set(CMAKE_BUILD_TYPE Release)

project(robomaster_meta_vision)

find_package(OpenCV REQUIRED)
add_subdirectory(lib/yaml-cpp)

file(GLOB_RECURSE SOURCES src/*.c src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h src/*.hpp)
add_executable(robomaster_meta_vision ${SOURCES} ${HEADERS})

target_link_libraries(
    robomaster_meta_vision
    ${OpenCV_LIBRARIES}
)
