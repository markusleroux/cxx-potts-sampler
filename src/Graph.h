//
// Created by Markus Le Roux on 2021-01-23.
//

#ifndef POTTSSAMPLER_GRAPH_H
#define POTTSSAMPLER_GRAPH_H

#include <iostream>
#include <vector>
#include <list>
#include <boost/dynamic_bitset.hpp>

class Graph {
    std::vector<std::vector<bool>> adjacencyMatrix;

	public:
        Graph(unsigned n, const std::list<std::pair<unsigned, unsigned>>& edges);

        unsigned getSize() const { return adjacencyMatrix.size(); }
        unsigned getEdgeCount() const;

        std::vector<bool> getNeighboursBool(unsigned v) const { return adjacencyMatrix[v]; }
        std::vector<unsigned int> getNeighboursIndex(unsigned v) const { return getIndexVector<std::vector<bool>>(getNeighboursBool((v))); }
        std::vector<std::vector<bool>> getAdjacencyMatrix() const { return adjacencyMatrix; }

		template <typename bitset_t>
		static std::vector<unsigned> getIndexVector(bitset_t bs) {
		    std::vector<unsigned> result;
		    for (int i = 0; i < bs.size(); i++) {
		        if (bs[i]) { result.push_back(i); }
		    }
		    return result;
		}
};

// Overload << to print vector contents
template <typename T>
static std::ostream& operator<<(std::ostream& out, const std::vector<T> &vector)
{
    for (unsigned value : vector) { out << value << ","; }
    return out;
}

// Overload << to print adjacency matrix
static std::ostream& operator<< (std::ostream &out, const Graph &graph) {
    for (unsigned v = 0; v < graph.getSize(); v++) {
        out << v << ": {" << graph.getNeighboursIndex(v) << "}\n";
    }
    return out;
}


#endif //POTTSSAMPLER_GRAPH_H
