find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(sampler cli.cpp)
target_link_libraries(sampler libpotts ${Boost_PROGRAM_OPTIONS_LIBRARY})
