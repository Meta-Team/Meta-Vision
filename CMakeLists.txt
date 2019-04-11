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

# Build MJPEGWriter (HTTP server for streaming images)
file(GLOB MJPEGWriter_SOURCES lib/MJPEGWriter/*.cpp)
file(GLOB MJPEGWriter_HEADERS lib/MJPEGWriter/*.h)
add_library(MJPEGWriter SHARED ${MJPEGWriter_SOURCES} ${MJPEGWriter_HEADERS})
target_link_libraries(MJPEGWriter ${OpenCV_LIBRARIES})
target_include_directories(MJPEGWriter PUBLIC lib/MJPEGWriter ${OpenCV_INCLUDE_DIRS})

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

# Use Doxygen to generate documents
find_package(Doxygen COMPONENTS dot OPTIONAL_COMPONENTS mscgen dia)
if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "RoboMaster Meta Vision")
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_PACKAGE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_HTML_OUTPUT "doc")
    set(DOXYGEN_QUIET YES)
    doxygen_add_docs(robomaster_meta_vision_docs ${SOURCES} ${HEADERS} ALL)
endif(DOXYGEN_FOUND)

target_link_libraries(
    robomaster_meta_vision
    ${OpenCV_LIBRARIES}
    yaml_cpp
    MJPEGWriter
    pthread
)
