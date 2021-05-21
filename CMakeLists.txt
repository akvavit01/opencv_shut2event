# Set CMake minimum version
cmake_minimum_required(VERSION 3.0.0)

# Set project name and version
project(opencv_shut2event VERSION 2.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include libraries
include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
)

set(PYDVS_LIBS 
    src/dvs_emu.cpp 
    src/dvs_op.cpp
)

set(SOURCES
    src/main.cpp
    ${PYDVS_LIBS}
)

# Create main object
add_executable(main ${SOURCES})

# Include main libraries
target_link_libraries(main PUBLIC
    ${OpenCV_LIBS}
)