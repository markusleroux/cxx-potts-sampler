//
// Created by Markus Le Roux on 2021-01-23.
//

#ifndef POTTSSAMPLER_GRAPH_H
#define POTTSSAMPLER_GRAPH_H


#include <vector>
#include <list>
#include <boost/dynamic_bitset.hpp>

class Graph {

    std::vector<std::vector<bool>> adjacencyMatrix;

    protected:
        template <typename bitset_t>
        std::vector<unsigned> getIndexVector(bitset_t bs) {
            std::vector<unsigned> result;
            for (int i = 0; i < bs.size(); i++) {
                if (bs[i]) { result.push_back(i); }
            }
            return result;
        }

    public:
        Graph(unsigned n, const std::list<std::pair<unsigned, unsigned>>& edges);

        std::vector<bool> getNeighboursBool(unsigned v) { return adjacencyMatrix[v]; }
        std::vector<unsigned int> getNeighboursIndex(unsigned v) { return getIndexVector<std::vector<bool>>(getNeighboursBool((v))); }
        std::vector<std::vector<bool>> getAdjacencyMatrix() { return adjacencyMatrix; }
};


#endif //POTTSSAMPLER_GRAPH_H
