# cmake_minimum_required(VERSION 3.1.0)
cmake_minimum_required(VERSION 3.8.0)


# set the project name
project(PottsSampler LANGUAGES CXX)

# output to bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)
