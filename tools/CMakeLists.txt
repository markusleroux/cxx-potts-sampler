find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(potts-sampler cli.cpp)
target_link_libraries(potts-sampler libpotts ${Boost_PROGRAM_OPTIONS_LIBRARY})
