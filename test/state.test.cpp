#include <catch2/catch_test_macros.hpp>
#include "state.hpp"


TEST_CASE("bounding list class", "[BoundingList]") {
    SECTION("initialization") {
        SECTION("copy initialization") {
            auto setIndices = std::vector<int>{1, 3, 4};
            BoundingList boundingList(5, setIndices);
            BoundingList copy = boundingList;
            REQUIRE(copy == boundingList);
        }

        SECTION("all zero initialization") {
            BoundingList boundingList(5);
            REQUIRE(boundingList.size() == 5);
            REQUIRE(boundingList.count() == 0);
        }

        SECTION("index based initialization") {
            auto setIndices = std::vector<int>{1, 3, 4};
            BoundingList boundingList(5, setIndices);

            REQUIRE(boundingList.size() == 5);
            REQUIRE(boundingList.count() == setIndices.size());
            for (auto index : setIndices) {
                REQUIRE(boundingList[index]);
            }
        }

    }

    SECTION("public methods") {
        BoundingList bl1(5, std::vector<int>{1});
        BoundingList bl2(5, std::vector<int>{1, 3});

        SECTION("bounding lists can be combined with boolean operators") {
            CHECK((bl1 | bl2) == bl2);
            CHECK((bl1 & bl2) == bl1);
        }

        SECTION("can be restricted to the first k set bits (rest of bits are zeroed)") {
            bl2.makeAtMostKSet(1);
            CHECK(bl2 == bl1);
        }

        SECTION("complement") {
            BoundingList bl3(5, std::vector<int>{0, 2, 3, 4});
            CHECK(bl1.flip_copy() == bl3);
        }
    }
}


TEST_CASE("queries", "[Queries]") {
    auto params = Parameters{5, 7, 3, 0.99};
    auto graph = Graph(params.numNodes, Graph::Type::CYCLE);
    BoundingList defaultBL(params.maxColours);
    defaultBL.set();

    State state{.parameters    = params,
                .graph         = graph,
                .colouring     = colouring_t(params.numNodes),
                .boundingChain = boundingchain_t(params.numNodes, defaultBL)};

    SECTION("get fixed or unfixed colours") {
        CHECK(queries::getUnfixedColours(state.graph, state.parameters, state.boundingChain, 0).all());

        state.boundingChain[2] = BoundingList(params.maxColours, std::vector<int>{1});
        state.boundingChain[4] = BoundingList(params.maxColours, std::vector<int>{3, 4});

        BoundingList expectedUnfixed(params.maxColours);
        expectedUnfixed.set(3);
        expectedUnfixed.set(4);

        SECTION("getUnfixedColours") {
            CHECK(queries::getUnfixedColours(state.graph, state.parameters, state.boundingChain, 3) == expectedUnfixed);
        }

        SECTION("getFixedColours") {
            CHECK(queries::getFixedColours(state.graph, state.parameters, state.boundingChain, 3) == expectedUnfixed.flip_copy());
        }
    }

    SECTION("getNeighbourhoodColourCount") {
        state.colouring[2] = 2;
        state.colouring[4] = 4;
        std::vector<int> expectedColourCount{0, 0, 1, 0, 1, 0, 0};
        CHECK(queries::getNeighbourhoodColourCount(state.graph, state.parameters, state.colouring, 3) == expectedColourCount);
    }

    SECTION("generate a set A") {
        CHECK(queries::getA(graph, state.parameters, state.boundingChain, 0, 3) \
                == BoundingList(params.maxColours, std::vector<int>{0, 1, 2}));

        state.boundingChain[2] = BoundingList(params.maxColours, std::vector<int>{1, 2});
        state.boundingChain[4] = BoundingList(params.maxColours, std::vector<int>{2, 3});
        CHECK(queries::getA(graph, state.parameters, state.boundingChain, 3, 5) \
                == BoundingList(params.maxColours, std::vector<int>{0, 1, 2, 3, 4}));
    }

    SECTION("compute Q") {
        for (int colour = 0; colour < params.maxColours; colour++) {
            CHECK(queries::m_Q(state.graph, state.parameters, state.boundingChain, 3, colour) == 0);
        }

        state.boundingChain[2] = BoundingList(params.maxColours, std::vector<int>{1});
        CHECK(queries::m_Q(state.graph, state.parameters, state.boundingChain, 3, 1) == 1);
    }
}
