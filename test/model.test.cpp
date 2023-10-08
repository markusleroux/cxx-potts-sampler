#include <vector>

#include "catch2/catch.hpp"
#include "test_interfaces.hpp"

TEST_CASE("graph class", "[Graph]") {
    SECTION("initialization") {
        SECTION("a cycle graph on two vertices") {
            Graph sCycleGraph(2, "cycle");

            REQUIRE(sCycleGraph.numEdges() == 1);
            REQUIRE(sCycleGraph.size() == 2);
            REQUIRE(sCycleGraph.getNeighboursIndex(0) == std::vector<int>{1});
            REQUIRE(sCycleGraph.getNeighboursIndex(1) == std::vector<int>{0});
        }

        SECTION("a cycle graph on five vertices") {
            Graph mCycleGraph(5, "cycle");

            REQUIRE(mCycleGraph.numEdges() == 5);
            REQUIRE(mCycleGraph.size() == 5);

            std::vector<int> neighbours;
            for (int v = 0; v < 5; v++) {
                neighbours = mCycleGraph.getNeighboursIndex(v);
                std::sort(neighbours.begin(), neighbours.end());

                std::vector<int> correctNeighbours{(v + 4) % 5, (v + 1) % 5};
                std::sort(correctNeighbours.begin(), correctNeighbours.end());

                REQUIRE(neighbours == correctNeighbours);
            }
        }

        SECTION("a complete graph on five vertices") {
            Graph mCompGraph(5, "complete");

            REQUIRE(mCompGraph.numEdges() == 5 * 4 / 2);
            REQUIRE(mCompGraph.size() == 5);

            std::vector<int> neighbours;
            std::vector<int> correctNeighbours;
            for (int v = 0; v < 5; v++) {
                neighbours = mCompGraph.getNeighboursIndex(v);
                std::sort(neighbours.begin(), neighbours.end());

                correctNeighbours.clear();
                for (int i = 0; i < 5; i++) {
                    if (i != v) {
                        correctNeighbours.emplace_back(i);
                    }
                }

                REQUIRE(neighbours == correctNeighbours);
            }
        }
    }
}

TEST_CASE("bounding lists class", "[BoundingList]") {
    SECTION("initialization") {}

    SECTION("methods") {
        BoundingList bl1(5, std::vector<int>{1});
        BoundingList bl2(5, std::vector<int>{1, 3});

        REQUIRE(bl1.size() == 5);
        REQUIRE(bl1.count() == 1);

        SECTION("bounding lists can be combined with boolean operators") {
            CHECK((bl1 | bl2) == bl2);
            CHECK((bl1 & bl2) == bl1);
        }

        SECTION("at most k bits are up") {
            bl2.atMostKUp(1);
            CHECK(bl2 == bl1);
        }

        SECTION("complement") {
            BoundingList bl3(5, std::vector<int>{0, 2, 3, 4});
            CHECK(bl1.C() == bl3);
        }

        SECTION("finite union") {
            BoundingList bl3(5, std::vector<int>{2});
            std::vector<BoundingList> ubl{bl1, bl2, bl3};
            BoundingList bl4(5, std::vector<int>{1, 2, 3});

            CHECK(BoundingList::unionOfLists(ubl, 5) == bl4);
        }
    }
}

TEST_CASE("model class", "[Model]") {
    auto params = std::shared_ptr<const Parameters>(new Parameters{5, 7, 3, 0.95});
    ModelTestInterface mdl(params, "cycle");
    SECTION("initialization") {
        SECTION("models initialize with full bounding chains") {
            BoundingList bl1(7);
            bl1.flip();

            for (const BoundingList &bl : mdl.getBoundingChain()) {
                REQUIRE(bl == bl1);
            }
        }
    }

    SECTION("methods") {
        SECTION("set the colour of a vertex") {
            SECTION("if the colour is inside the bounding list") {
                mdl.setColour_p(0, 3);
                REQUIRE(mdl.getColour(0) == 3);
            }
            SECTION("ONLY if the colour is inside the bounding list") {
                REQUIRE_THROWS(mdl.setColour_p(0, 8));

                mdl.setBoundingList_p(4, std::vector<int>{1});
                REQUIRE_THROWS(mdl.setColour_p(4, 2));
            }
        }

        SECTION("generate a set A") {
            CHECK(mdl.bs_generateA_p(0, 3) == BoundingList(7, std::vector<int>{0, 1, 2}));

            mdl.setBoundingList_p(2, std::vector<int>{1, 2});
            mdl.setBoundingList_p(4, std::vector<int>{2, 3});
            CHECK(mdl.bs_generateA_p(3, 5) == BoundingList(7, std::vector<int>{0, 1, 2, 3, 4}));
        }

        SECTION("compute m_Q") {
            for (int c = 0; c < 7; c++) {
                CHECK(mdl.m_Q_p(3, c) == 0);
            }

            mdl.setBoundingList_p(2, std::vector<int>{1});
            CHECK(mdl.m_Q_p(3, 1) == 1);
        }

        SECTION("bs_getUnfixedColours") {
            CHECK(mdl.bs_getUnfixedColours_p(0).all());

            mdl.setBoundingList_p(2, std::vector<int>{1});
            mdl.setBoundingList_p(4, std::vector<int>{3, 4});
            BoundingList result(7);
            result.set(3);
            result.set(4);
            CHECK(mdl.bs_getUnfixedColours_p(3) == result);
        }

        SECTION("getNeighbourhoodColourCount") {
            mdl.setColour_p(2, 2);
            mdl.setColour_p(4, 4);
            std::vector<int> result{0, 0, 1, 0, 1, 0, 0};
            CHECK(mdl.getNeighbourhoodColourCount(3) == result);
        }
    }
}

TEST_CASE("get indices from bitset", "[getIndexVector][BoundingList]") {
    SECTION("vector of booleans") {
        std::vector<bool> bv(5);
        bv[2] = true;
        bv[4] = true;
        REQUIRE(getIndexVector<std::vector<bool>>(bv) == std::vector<int>{2, 4});
    }

    SECTION("bounding list") {
        BoundingList bl(5);
        bl.set(2);
        bl.set(4);
        REQUIRE(getIndexVector<BoundingList>(bl) == std::vector<int>{2, 4});
    }
}
