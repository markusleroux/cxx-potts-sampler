cmake_minimum_required(VERSION 3.1.0)

# set the project name
project(PottsSampler)

# set C++ version
set(CMAKE_CXX_STANDARD 17)

# output to bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

include_directories(/usr/local/Cellar/boost/1.75.0_1/include)
add_executable(sampler src/main.cpp src/Model.cpp src/Model.h src/Graph.cpp src/Graph.h src/Compress.cpp src/Compress.h src/Update.h src/Update.cpp src/Contract.cpp src/Contract.h)
