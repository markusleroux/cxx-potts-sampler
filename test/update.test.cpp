#include <set>

#include <catch2/catch_test_macros.hpp>
#include "random.hpp"
#include "update.hpp"


TEST_CASE("helpers", "[Update]") {
    SECTION("compute power") {
        std::vector<int> counts{0, 1, 2, 3};
        std::vector<long double> result{1, 0.5, 0.25, 0.125};
        CHECK(pow(0.5, counts) == result);
    }
}

TEST_CASE("compress class", "[Compress]") {
    auto params = Parameters{5, 7, 3, 0.99};
    auto graph = Graph(params.numNodes, Graph::Type::CYCLE);
    BoundingList defaultBL(params.maxColours);
    defaultBL.set();

    State state{.parameters    = params,
                .graph         = graph,
                .colouring     = colouring_t(params.numNodes),
                .boundingChain = boundingchain_t(params.numNodes, defaultBL)};

    SECTION("public methods")
    {
        BoundingList boundingList(params.maxColours);
        boundingList.set(0), boundingList.set(1), boundingList.set(2);

        CompressUpdate compressUpdate(state, 3, boundingList);

        SECTION("colour1 (c1) is added to bounding list") {
            boundingList.set(compressUpdate.c1);
            REQUIRE(compressUpdate.getNewBoundingChain() == boundingList);
        }

        SECTION("new colour is in bounding list") {
            boundingList.set(compressUpdate.c1);

            auto colour = compressUpdate.getNewColour();
            REQUIRE(std::set<int>{0, 1, 2, compressUpdate.c1}.count(colour) == 1);
        }
    }
}

TEST_CASE("contract class", "[Contract]") {
    auto params = Parameters{5, 7, 3, 0.99};
    auto graph = Graph(params.numNodes, Graph::Type::CYCLE);
    BoundingList defaultBL(params.maxColours);
    defaultBL.set();

    State state{.parameters    = params,
                .graph         = graph,
                .colouring     = colouring_t(params.numNodes),
                .boundingChain = boundingchain_t(params.numNodes, defaultBL)};

    SECTION("public methods") {
        BoundingList boundingList(params.maxColours);
        boundingList.set(0), boundingList.set(1), boundingList.set(2);

        CompressUpdate compressUpdateNode1(state, 1, boundingList);
        CompressUpdate compressUpdateNode4(state, 4, boundingList);
        ContractUpdate contractUpdate(state, 0);

        SECTION("bounding list is either {c2} or {c1, c2}") {
            BoundingList bl(params.maxColours);
            bl.set(contractUpdate.c1), bl.set(contractUpdate.c2);

            BoundingList newBoundingList = contractUpdate.getNewBoundingChain();
            REQUIRE(newBoundingList <= bl);
            REQUIRE(newBoundingList.count() >= 1);
        }

        SECTION("new colour is in bounding list") {
            int colour = contractUpdate.getNewColour();
            REQUIRE(std::set<int>{contractUpdate.c1, contractUpdate.c2}.count(colour) == 1);
        }
    }
}
