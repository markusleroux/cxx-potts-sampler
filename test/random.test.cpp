#include <catch2/catch_test_macros.hpp>

#include "state.hpp"
#include "random.hpp"

#include <set>


TEST_CASE("update class", "[Update]") {
    SECTION("methods") {
        SECTION("sample from bounding list") {
            BoundingList bl(7);
            bl.set(1), bl.set(3), bl.set(5);
            for (int i = 0; i < 10; i++) {
                CHECK(std::set{1, 3, 5}.count(uniformSample(bl)) == 1);
            }
        }
    }
}
