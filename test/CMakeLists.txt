find_package(Catch2 REQUIRED)

add_executable(tests
    test_interfaces.hpp
    main.cpp
    model.test.cpp
    update.test.cpp
    sampler.test.cpp
)
target_link_libraries(tests
    PRIVATE libpotts
    PRIVATE Catch2::Catch2
)
target_include_directories(tests
    PRIVATE $<TARGET_PROPERTY:libpotts,INCLUDE_DIRECTORIES>
)

