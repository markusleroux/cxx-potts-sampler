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

/// template which returns the indices of the set bits
/// \tparam bitset_t the type of the bitset
/// \param bs
/// \return the indices of set bits in bs
template <typename bitset_t>
static std::vector<int> getIndexVector(bitset_t bs) {
	std::vector<int> result;
	for (int i = 0; i < bs.size(); i++) {
		if (bs[i]) {
			result.push_back(i);
		}
	}
	return result;
}

class BoundingList : public boost::dynamic_bitset<> {
		public:
				BoundingList(int q, const std::vector<int> &boundingList);
				explicit BoundingList(int q);
				BoundingList() = default;

				void atMostKUp(int k);
				static BoundingList unionOfLists(const std::vector<BoundingList> &lists, int q);

				BoundingList C() const;
};

typedef std::vector<std::vector<bool>> adjmatrix_t;
typedef std::pair<int, int> edge_t;
typedef std::vector<BoundingList> boundingchain_t;
typedef std::vector<int> colouring_t;

/// \brief A class for graphs as adjacency matrices
class Graph {
		private:
				adjmatrix_t adjacencyMatrix;

		public:
				Graph(int n, const std::__1::list<edge_t> &edges);

				Graph(int n, const std::string &type);

				/// get the number of vertices in the graph
				int getSize() const { return static_cast<int>(adjacencyMatrix.size()); }

				int getEdgeCount() const;

				/// getter for column v of the adjacency matrix
				std::vector<bool> getNeighboursBool(int v) const {
					return adjacencyMatrix[v];
				}

				/// a method which gets the indices of the neighbours of v
				std::vector<int> getNeighboursIndex(int v) const {
					return getIndexVector<std::vector<bool>>(getNeighboursBool((v)));
				}

				static std::__1::list<edge_t> buildEdgeSet(int n,
				                                           const std::string &type);
};

/// \brief a class for holding parameters of the model, the colourings and the
/// bounding chain
class Model : public Graph {
private:
  colouring_t colouring;
  boundingchain_t boundingChain;
  int q;
  int Delta;
  long double B;
  static std::mt19937 mersene_gen;
  bool checkBoundingList = true;

  BoundingList bs_getUnfixedColours(int v) const;

  BoundingList bs_getFixedColours(int v) const;

  int m_Q(int v, int c) const;

  void setColour(int v, int c);

  /// setter for the checkBoundingList property
  void setBoundingListChecks(bool b) { checkBoundingList = b; }

  /// setter for bounding list
  void setBoundingList(int v,
                       const std::vector<int> &boundingList);

  BoundingList bs_generateA(int v, int size) const;

  friend class Update;

  friend class Compress;

  friend class Contract;

  friend class Sampler;

  friend class ModelTest;

public:
  Model(int n, int q, int Delta, long double B,
        const std::list<edge_t> &edges);

  Model(int n, int q, int Delta, long double B,
        const std::string &type);

  /// get the colour of a specific vertex
  int getColour(int v) const { return colouring[v]; }

  /// get the current colouring of the graph
  colouring_t getColouring() { return colouring; }

  std::vector<int> getUnfixedColours(int v) const;

  std::vector<int> getFixedColours(int v) const;

  std::vector<int> getNeighbourhoodColourCount(int v) const;

	/// getter for the bounding chain
	boundingchain_t getBoundingChain() const { return boundingChain; }

	std::vector<BoundingList> getBoundingLists(const std::vector<int> &vertices) const;

	/// getter for the bounding list at vertex v
	BoundingList getBoundingList(int v) const {
		return boundingChain[v];
	}

  std::vector<int> getBoundingListIndex(int v) const {
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
  for (int v = 0; v < graph.getSize(); v++) {
    out << v << ": {" << graph.getNeighboursIndex(v) << "}\n";
  }
  return out;
}

// Overload << for model
static std::ostream &operator<<(std::ostream &out, const Model &model) {
	for (int v = 0; v < model.getSize(); v++) {
		out << v << " (" << model.getColour(v) << ") : {" << model.getNeighboursIndex(v) << "}\n";
	}
	return out;
}

#endif // POTTSSAMPLER_MODEL_H
