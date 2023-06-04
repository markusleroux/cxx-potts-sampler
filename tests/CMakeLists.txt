find_package(Catch2 REQUIRED)

add_executable(tests
        main.cpp
        model.test.cpp
        update.test.cpp
        sampler.test.cpp 
        TestClasses.h
        )

target_link_libraries(tests src Catch2::Catch2)
target_compile_features(tests PUBLIC cxx_std_17)
set_target_properties(tests PROPERTIES CXX_EXTENSIONS OFF)
