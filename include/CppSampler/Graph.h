//
// Created by Markus Le Roux on 2021-01-23.
//

#ifndef POTTSSAMPLER_GRAPH_H
#define POTTSSAMPLER_GRAPH_H

#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <list>
#include <vector>

typedef std::vector<std::vector<bool>> adjmatrix_t;
typedef std::pair<unsigned, unsigned> edge_t;

/// \brief A class for graphs as adjacency matrices
class Graph {
private:
  adjmatrix_t adjacencyMatrix;

protected:
  /// getter for column v of the adjacency matrix
  std::vector<bool> getNeighboursBool(unsigned v) const {
    return adjacencyMatrix[v];
  }

  /// template which returns the indices of the set bits
  /// \tparam bitset_t the type of the bitset
  /// \param bs
  /// \return the indices of set bits in bs
  template <typename bitset_t>
  static std::vector<unsigned> getIndexVector(bitset_t bs) {
    std::vector<unsigned> result;
    for (unsigned i = 0; i < bs.size(); i++) {
      if (bs[i]) {
        result.push_back(i);
      }
    }
    return result;
  }

public:
  Graph(unsigned n, const std::list<edge_t> &edges);

  Graph(unsigned int n, const std::string &type);

  /// get the number of vertices in the graph
  unsigned getSize() const { return adjacencyMatrix.size(); }

  unsigned getEdgeCount() const;

  /// a method which gets the indices of the neighbours of v
  std::vector<unsigned int> getNeighboursIndex(unsigned v) const {
    return getIndexVector<std::vector<bool>>(getNeighboursBool((v)));
  }

  static std::list<edge_t> buildEdgeSet(unsigned int n,
                                        const std::string &type);
};

/// Overload << to print vector contents
template <typename element_type>
static std::ostream &operator<<(std::ostream &out,
                                const std::vector<element_type> &vector) {
  if (not vector.empty()) {
    for (auto it = std::begin(vector); it != std::prev(std::end(vector));
         it++) {
      out << *it << ",";
    }
    out << *std::prev(std::end(vector));
  }
  return out;
}

/// Overload << to print adjacency matrix
static std::ostream &operator<<(std::ostream &out, const Graph &graph) {
  for (unsigned v = 0; v < graph.getSize(); v++) {
    out << v << ": {" << graph.getNeighboursIndex(v) << "}\n";
  }
  return out;
}

#endif // POTTSSAMPLER_GRAPH_H
