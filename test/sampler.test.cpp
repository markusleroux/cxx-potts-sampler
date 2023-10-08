#include <set>

#include "catch2/catch.hpp"
#include "test_interfaces.hpp"

TEST_CASE("sampler class", "[Sampler]") {
    auto params = std::shared_ptr<const Parameters>(new Parameters{5, 7, 3, 0.95});
    ModelTestInterface mdl(params, "cycle");
    SamplerTestInterface smplr(mdl);

    SECTION("methods") {
        SECTION("bounding chain is constant") {
            REQUIRE_FALSE(smplr.boundingChainIsConstant_p());

            for (int v = 0; v < 5; v++) {
                mdl.setBoundingList_p(v, std::vector<int>{v});
            }
            REQUIRE(smplr.boundingChainIsConstant_p());
        }

        SECTION("update with seeds") {
            BoundingList A0(7);
            A0.set(0), A0.set(1), A0.set(2);

            CompressTestInterface comU1(mdl, 1, A0);
            CompressTestInterface comU4(mdl, 4, A0);
            ContractTestInterface conU(mdl, 0);

            iteration_t seeds{comU1, comU4, conU};
            smplr.updateColourWithSeeds_p(seeds);

            REQUIRE(std::set<int>{conU.getC1(), conU.getC2()}.count(mdl.getColour(0)) == 1);

            REQUIRE(std::set<int>{0, 1, 2, comU1.getC1()}.count(mdl.getColour(1)) == 1);
            REQUIRE(std::set<int>{0, 1, 2, comU4.getC1()}.count(mdl.getColour(4)) == 1);
        }

        SECTION("run an iteration") { REQUIRE_NOTHROW(smplr.iteration_p()); }

        SECTION("generate a sample") { REQUIRE_NOTHROW(smplr.sample()); }
    }
}
