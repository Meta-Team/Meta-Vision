cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS_DEBUG "-Wall -g" CACHE STRING "")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE "-Wall -O3" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3" CACHE STRING "")
set(CMAKE_BUILD_TYPE Debug)

project(Meta_Vision)

# Build OpenCV
set(OpenCV_DIR "C://opencv//sources//build")
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

# Build Nanopb
# set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/nanopb/extra)
# find_package(Nanopb REQUIRED)
# include_directories(${NANOPB_INCLUDE_DIRS})
# include_directories(${CMAKE_CURRENT_BINARY_DIR})
# nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS src/serial/serial.proto)
# set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

# Include boost (for macOS to find the package)
find_package(Boost REQUIRED COMPONENTS program_options filesystem)
include_directories(${Boost_INCLUDE_DIRS})
MESSAGE("Boost information:")
MESSAGE("  Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
MESSAGE("  Boost_LIBRARIES: ${Boost_LIBRARIES}")
MESSAGE("  Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")

# Build RM Vision software
file(GLOB_RECURSE VISION_SOURCES src/*.c src/*.cpp)
file(GLOB_RECURSE VISION_HEADERS src/*.h src/*.hpp)
add_executable(Vision ${VISION_SOURCES} ${VISION_HEADERS})
# add_executable(Vision ${SOURCES} ${HEADERS} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(Vision
        ${OpenCV_LIBRARIES}
        yaml_cpp
        MJPEGWriter
        pthread
        )

# RM2019 Xidian SVM training exectuables
file(GLOB_RECURSE ARMOR_SAMPLE_SOURCES utilities/armor_sample/*.c utilities/armor_sample/*.cpp)
file(GLOB_RECURSE ARMOR_SAMPLE_HEADERS utilities/armor_sample/*.h utilities/armor_sample/*.hpp)
add_executable(Armor_Sample ${ARMOR_SAMPLE_SOURCES} ${ARMOR_SAMPLE_HEADERS} utilities/camera/PrtSc.cpp)
target_link_libraries(Armor_Sample ${OpenCV_LIBRARIES} Boost::program_options Boost::filesystem)

file(GLOB_RECURSE SVM_TRAINER_SOURCES utilities/train/*.c utilities/train/*.cpp)
file(GLOB_RECURSE SVM_TRAINER_HEADERS utilities/train/*.h utilities/train/*.hpp)
add_executable(SVM_Trainer ${SVM_TRAINER_SOURCES} ${SVM_TRAINER_HEADERS})
target_link_libraries(SVM_Trainer ${OpenCV_LIBRARIES})

file(GLOB_RECURSE CAMERA_CALIBRATION_SOURCES utilities/camera_calibration/*.c utilities/camera_calibration/*.cpp)
file(GLOB_RECURSE CAMERA_CALIBRATION_HEADERS utilities/camera_calibration/*.h utilities/camera_calibration/*.hpp)
add_executable(Camera_Calibration ${CAMERA_CALIBRATION_SOURCES} ${CAMERA_CALIBRATION_HEADERS})
target_link_libraries(Camera_Calibration ${OpenCV_LIBRARIES})

file(GLOB_RECURSE CAMERA_PRTSC_SOURCES utilities/camera/*.cpp)
add_executable(Camera_PrtSc  ${CAMERA_PRTSC_SOURCES})
target_link_libraries(Camera_PrtSc ${OpenCV_LIBRARIES})

# Use Doxygen to generate documents
find_package(Doxygen COMPONENTS dot OPTIONAL_COMPONENTS mscgen dia)
if (DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "RoboMaster Meta Vision")
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_PACKAGE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_HTML_OUTPUT "doc")
    set(DOXYGEN_QUIET YES)
    set(DOXYGEN_ENABLE_PREPROCESSING YES)
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
    set(DOXYGEN_PREDEFINED "__attribute__(x)=")
    doxygen_add_docs(Vision_Docs ${SOURCES} ${HEADERS} ALL)
endif (DOXYGEN_FOUND)