
cmake_minimum_required(VERSION 2.8)

PROJECT(Optimization)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

SET(SRC_LIST main.cpp optimization.h optimization.cpp vector4.h vector4.cpp)

ADD_EXECUTABLE(Optimization ${SRC_LIST})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

TARGET_LINK_LIBRARIES(Optimization ${VTK_LIBRARIES})
