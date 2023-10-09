#ifndef POTTSSAMPLER_SAMPLER_H
#define POTTSSAMPLER_SAMPLER_H

#include <iostream>
#include <string>
#include <vector>

struct Parameters {
    // Number of nodes in the graph
    int numNodes;

    // Number of distinct colours
    int maxColours;

    // Maximum degree
    int maxDegree;

    // Temperature
    long double temperature;

    bool verify() const;
};

class Graph {
   public:
    enum Type { CYCLE, COMPLETE };

    using edge_t = std::pair<int, int>;

    Graph(int numNodes, Type type) : Graph(numNodes, buildEdgeSet(numNodes, type)) {}

    Graph(int nunNodes, const std::vector<edge_t>& edges);

    int size() const { return adjacencyMatrix.size(); }

    int numEdges() const;

    const std::vector<int>& getNeighbours(int v) const { return adjacencyMatrix[v]; }

    friend std::ostream& operator<<(std::ostream& out, const Graph& graph);

   protected:
    static std::vector<edge_t> buildEdgeSet(int n, Type type);

    std::vector<std::vector<int>> adjacencyMatrix;
};

std::istream& operator>>(std::istream& is, Graph::Type& type);

using colouring_t = std::vector<int>;

/// sample from the anti-ferromagnetic Potts model
colouring_t sample(const Parameters& parameters, const Graph& graph);

#endif
