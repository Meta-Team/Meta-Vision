cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS_DEBUG "-Wall -g" CACHE STRING "")       
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE "-Wall -O3" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3" CACHE STRING "")
set(CMAKE_BUILD_TYPE Debug)

project(robomaster_meta_vision)

# Build OpenCV
find_package(OpenCV REQUIRED)

# Build YAML-CPP
file(GLOB YAML_CPP_SOURCES lib/yaml-cpp/src/*.cpp)
file(GLOB YAML_CPP_HEADERS lib/yaml-cpp/src/*.h)
add_library(yaml_cpp SHARED ${YAML_CPP_SOURCES} ${YAML_CPP_HEADERS})
target_include_directories(yaml_cpp PUBLIC lib/yaml-cpp/include)
target_compile_definitions(yaml_cpp PUBLIC -DYAML_CPP_NO_CONTRIB)

# Build RM Vision software
file(GLOB_RECURSE SOURCES src/*.c src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h src/*.hpp)
add_executable(robomaster_meta_vision ${SOURCES} ${HEADERS})
target_link_libraries(robomaster_meta_vision pthread)

target_link_libraries(
    robomaster_meta_vision
    yaml_cpp
    ${OpenCV_LIBRARIES}
)
