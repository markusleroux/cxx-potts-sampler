
add_executable(sampler main.cpp
                       Model.cpp
                       Graph.cpp
                       Compress.cpp
                       Contract.cpp
                       Update.cpp
                       Sampler.cpp)

target_link_libraries(sampler Boost::program_options)
