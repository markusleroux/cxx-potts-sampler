add_library(libpotts
    sampler.cpp
    state.hpp state.cpp
    update.hpp update.cpp
    random.hpp random.cpp
)
target_link_libraries(libpotts ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_include_directories(libpotts
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    PRIVATE .
)

