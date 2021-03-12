# Set CMake minimum version
cmake_minimum_required(VERSION 3.0.0)

# Set project name and version
project(opencv_shut2event VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)

# Include and enable CTest
include(CTest)
enable_testing()

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Find Eigen3
find_package(Eigen3)

# Find Qt5
find_package(Qt5 COMPONENTS 
    Core 
    Widgets 
    Test
    Concurrent 
    OpenGL 
)

# Find VTK
find_package(VTK)

# Find gflags
find_package(gflags)

# Find Intel TBB
find_package(TBB REQUIRED)

# Include headers
include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
    ${TBB_INCLUDE_DIR}
)

# Create main object
add_executable(main src/main.cpp)

# Include main libraries
target_link_libraries(main PUBLIC
    ${OpenCV_LIBS}
    ${TBB_LIBRARY}
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
