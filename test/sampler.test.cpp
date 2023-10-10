#include <set>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "sampler.hpp"
#include "state.hpp"


TEST_CASE("graph class", "[Graph]") {
    SECTION("initialization") {
        SECTION("a cycle graph on two vertices") {
            Graph sCycleGraph(2, Graph::Type::CYCLE);

            REQUIRE(sCycleGraph.size() == 2);
            REQUIRE(sCycleGraph.numEdges() == 1);
            REQUIRE(sCycleGraph.getNeighbours(0) == std::vector<int>{1});
            REQUIRE(sCycleGraph.getNeighbours(1) == std::vector<int>{0});
            REQUIRE(sCycleGraph.getMaxDegree() == 1);
        }

        SECTION("a cycle graph on five vertices") {
            static constexpr int numNodes = 5;
            Graph mCycleGraph(numNodes, Graph::Type::CYCLE);

            REQUIRE(mCycleGraph.size() == 5);
            REQUIRE(mCycleGraph.numEdges() == 5);

            for (int v = 0; v < numNodes; v++) {
                auto neighboursVec = mCycleGraph.getNeighbours(v);
                std::set<int> neighbours{neighboursVec.begin(), neighboursVec.end()};

                std::set<int> correctNeighbours{(v + numNodes - 1) % numNodes, (v + 1) % numNodes};
                REQUIRE(neighbours == correctNeighbours);
            }
            REQUIRE(mCycleGraph.getMaxDegree() == 2);
        }

        SECTION("a complete graph on five vertices") {
            static constexpr int numNodes = 5;
            Graph mCompGraph(numNodes, Graph::Type::COMPLETE);

            REQUIRE(mCompGraph.numEdges() == numNodes * (numNodes - 1) / 2);
            REQUIRE(mCompGraph.size() == numNodes);

            for (int v = 0; v < numNodes; v++) {
                auto neighboursVec = mCompGraph.getNeighbours(v);
                std::set<int> neighbours{neighboursVec.begin(), neighboursVec.end()};

                std::set<int> correctNeighbours;
                for (int i = 0; i < numNodes; i++) {
                    if (i != v) {
                        correctNeighbours.insert(i);
                    }
                }

                REQUIRE(neighbours == correctNeighbours);
            }
            REQUIRE(mCompGraph.getMaxDegree() == 4);
        }
    }
}


TEST_CASE("sampler class", "[Sampler]") {
    auto params = Parameters{5, 7, 0.95};
    auto graph = Graph(params.numNodes, Graph::Type::CYCLE);

    SECTION("methods") {
        SECTION("generate a sample") {
            REQUIRE_NOTHROW(sample(params, graph));
        }
    }
}

