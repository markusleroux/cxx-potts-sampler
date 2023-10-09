cmake_minimum_required(VERSION 3.8.0)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0
    FIND_PACKAGE_ARGS 3
)

set(BOOST_INCLUDE_LIBRARIES program_options)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.83.0
  FIND_PACKAGE_ARGS 1.83
)

FetchContent_MakeAvailable(Boost)

option(BUILD_CLI "Build the cli potts sampler tool" ON)

project(PottsSampler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(lib)

if(BUILD_CLI)
    add_subdirectory(tools)
endif()

if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
    FetchContent_MakeAvailable(Catch2)

    include(CTest)
    include(Catch)

    add_subdirectory(test)
endif()
