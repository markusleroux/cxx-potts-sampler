//
// Created by Markus Le Roux on 2021-01-23.
//

#ifndef POTTSSAMPLER_MODEL_H
#define POTTSSAMPLER_MODEL_H

#include <random>
#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <list>
#include <vector>

// TODO:
//  - move Graph::getIndexVector

class BoundingList : public boost::dynamic_bitset<> {
		public:
				BoundingList(unsigned q, const std::vector<unsigned> &boundingList);
				explicit BoundingList(unsigned q);
				BoundingList() = default;

				void atMostKUp(unsigned int k);
				static BoundingList unionOfLists(const std::vector<BoundingList> &lists, unsigned q);

				BoundingList C() const;
};

typedef std::vector<std::vector<bool>> adjmatrix_t;
typedef std::pair<unsigned, unsigned> edge_t;
typedef std::vector<BoundingList> boundingchain_t;
typedef std::vector<unsigned> colouring_t;

/// \brief A class for graphs as adjacency matrices
class Graph {
		private:
				adjmatrix_t adjacencyMatrix;

		protected:
				/// getter for column v of the adjacency matrix
				std::vector<bool> getNeighboursBool(unsigned v) const {
					return adjacencyMatrix[v];
				}

		public:
				Graph(unsigned n, const std::__1::list<edge_t> &edges);

				Graph(unsigned int n, const std::string &type);

				/// get the number of vertices in the graph
				unsigned getSize() const { return adjacencyMatrix.size(); }

				unsigned getEdgeCount() const;

				/// a method which gets the indices of the neighbours of v
				std::vector<unsigned int> getNeighboursIndex(unsigned v) const {
					return getIndexVector<std::vector<bool>>(getNeighboursBool((v)));
				}

				static std::__1::list<edge_t> buildEdgeSet(unsigned int n,
				                                           const std::string &type);

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
};

/// \brief a class for holding parameters of the model, the colourings and the
/// bounding chain
class Model : public Graph {
private:
  //      TODO: consider:
  //       - Model::Delta -> Graph::Delta
  //       - Model::B -> Sampler::B

  colouring_t colouring;
  boundingchain_t boundingChain;
  unsigned int q;
  unsigned int Delta;
  long double B;
  static std::mt19937 mersene_gen;
  bool checkBoundingList = true;

  BoundingList bs_getUnfixedColours(unsigned int v) const;

  BoundingList bs_getFixedColours(unsigned int v) const;

  unsigned int m_Q(unsigned int v, unsigned int c) const;

  /// setter for the checkBoundingList property
  void setBoundingListChecks(bool b) { checkBoundingList = b; }

  /// setter for bounding list
  void setBoundingList(unsigned int v,
                       const std::vector<unsigned int> &boundingList);

  std::vector<BoundingList> getBoundingLists(const std::vector<unsigned int> &vertices) const;

  /// getter for the bounding list at vertex v
  BoundingList getBoundingList(unsigned int v) const {
    return boundingChain[v];
  }

  /// getter for the bounding chain
  boundingchain_t getBoundingChain() const { return boundingChain; }

  BoundingList bs_generateA(unsigned int v, unsigned int size) const;

  friend class Update;

  friend class Compress;

  friend class Contract;

  friend class Sampler;

public:
  Model(unsigned int n, unsigned int q, unsigned int Delta, long double B,
        const std::list<edge_t> &edges);

  Model(unsigned int n, unsigned int q, unsigned int Delta, long double B,
        const std::string &type);

  /// get the colour of a specific vertex
  int getColour(unsigned int v) const { return colouring[v]; }

  void setColour(unsigned int v, unsigned int c);

  /// get the current colouring of the graph
  colouring_t getColouring() { return colouring; }

  std::vector<unsigned int> getUnfixedColours(unsigned int v) const;

  std::vector<unsigned int> getFixedColours(unsigned int v) const;

  std::vector<unsigned int> getNeighbourhoodColourCount(unsigned int v) const;

  std::vector<unsigned> getBoundingListIndex(unsigned int v) const {
    return getIndexVector(getBoundingList(v));
  }

  void sample();
};

/// Overload << to print vector contents
template <typename element_type>
static std::ostream &operator<<(std::ostream &out,
                                const std::__1::vector<element_type> &vector) {
  if (not vector.empty()) {
    for (auto it = std::begin(vector); it != std::prev(std::end(vector));
         it++) {
      out << *it << ",";
    }
    out << *std::prev(std::end(vector));
  }
  return out;
}

/// Overload << to for graph (adjacency matrix)
static std::ostream &operator<<(std::ostream &out, const Graph &graph) {
  for (unsigned v = 0; v < graph.getSize(); v++) {
    out << v << ": {" << graph.getNeighboursIndex(v) << "}\n";
  }
  return out;
}

// Overload << for model
static std::ostream &operator<<(std::ostream &out, const Model &model) {
	for (unsigned v = 0; v < model.getSize(); v++) {
		out << v << " (" << model.getColour(v) << ") : {" << model.getNeighboursIndex(v) << "}\n";
	}
	return out;
}

#endif // POTTSSAMPLER_MODEL_H
