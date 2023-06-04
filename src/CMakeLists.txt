find_package(Boost COMPONENTS program_options REQUIRED)

add_library(src
        Model.cpp
        Update.cpp
        Sampler.cpp)

target_link_libraries(src ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_compile_features(src PUBLIC cxx_std_17)
set_target_properties(src PROPERTIES CXX_EXTENSIONS OFF)


add_executable(sampler main.cpp)

target_link_libraries(sampler src ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_compile_features(sampler PUBLIC cxx_std_17)
set_target_properties(sampler PROPERTIES CXX_EXTENSIONS OFF)
