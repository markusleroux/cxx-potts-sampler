
add_executable(sampler main.cpp
                       Model.cpp
                       Update.cpp
                       Sampler.cpp)

target_link_libraries(sampler Boost::program_options)
