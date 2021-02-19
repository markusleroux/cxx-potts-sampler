# cmake_minimum_required(VERSION 3.1.0)
cmake_minimum_required(VERSION 3.8.0)


# set the project name
project(PottsSampler LANGUAGES CXX)

# set C++ version
# set(CMAKE_CXX_STANDARD 17)

# output to bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

add_subdirectory(src)
target_include_directories(sampler PUBLIC include/CppSampler)
target_compile_features(sampler PUBLIC cxx_std_17)
set_target_properties(sampler PROPERTIES CXX_EXTENSIONS OFF)
