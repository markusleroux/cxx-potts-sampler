add_executable(tests
    update.test.cpp
    sampler.test.cpp
    state.test.cpp
    random.test.cpp
)
target_link_libraries(tests PRIVATE libpotts Catch2::Catch2WithMain)
target_include_directories(tests
    PRIVATE $<TARGET_PROPERTY:libpotts,INCLUDE_DIRECTORIES>
)
catch_discover_tests(tests)

