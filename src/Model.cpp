#include "Model.h"
#include "Sampler.h"
#include <boost/dynamic_bitset.hpp>

BoundingList::BoundingList(const int q,
                           const std::vector<int> &boundingList)
    : boost::dynamic_bitset<>((unsigned long) q) {
  for (int colour : boundingList) {
    if (colour >= 0 && colour < q) {
      set(static_cast<unsigned long>(colour));
    } else {
      throw std::invalid_argument(
          "Bounding list must be a subset of {0, ..., q - 1}");
    }
  }
}

BoundingList::BoundingList(const int q) : boost::dynamic_bitset<>((unsigned long) q) {}

/// unset all bits after the kth set bit
/// \sa bs_generateA
/// \param k the number of bits set in after atMostKUp has been applied
/// \return a copy of bs with all set bits after the kth set bit unset
void BoundingList::atMostKUp(int k) {
  for (int i = 0; i < size(); i++) {
    if (test(static_cast<unsigned long>(i)) && k == 0) {
      reset(static_cast<unsigned long>(i));
    } else if (test(static_cast<unsigned long>(i))) {
      k--;
    }
  }
}

/// Return the union of the bounding lists at all the vertices
/// \param vertices
/// \return the bitwise OR of the bounding list bitsets
BoundingList
BoundingList::unionOfLists(const std::vector<BoundingList> &lists, int q) {
  BoundingList result{q};
  for (const BoundingList& bl : lists) {
    result |= bl;
  }
  return result;
}

// \brief analogous to flip but not in place
BoundingList BoundingList::C() const {
	BoundingList result{*this};
	result.flip();
	return result;
}

/// a seed for random number generators
std::mt19937 Model::mersene_gen{std::random_device{}()};

/// Constructor for the model class
/// \param n the number of vertices
/// \param q the number of colours
/// \param Delta the maximum degree of the graph
/// \param B controls the strength of the interactions between vertices
/// \param edges
Model::Model(int n, int q, int Delta, long double B,
             const std::list<edge_t> &edges)
    : Graph(n, edges), q(q), Delta(Delta), B(B) {
  // Initialize vector of colours
  colouring = std::vector<int>(static_cast<unsigned long>(n));

	// Initialize bounding chain
	BoundingList defaultBL(q);
	defaultBL.set();
	boundingChain = boundingchain_t(static_cast<unsigned long>(n), defaultBL);
}

/// constructor for some generic graphs
/// \param type type of graph, may be one of {"cycle", "complete"}
/// \param n the number of vertices in the graph
/// \param q the number of colours
/// \param Delta the maximum degree of the graph
/// \param B the strength of the interactions between vertices
/// \return a model object
Model::Model(int n, int q, int Delta, long double B,
             const std::string &type)
    : Graph(n, type), q(q), Delta(Delta), B(B) {
  // Initialize vector of colours
  colouring = colouring_t(static_cast<unsigned long>(n));

  // Initialize bounding chain
	BoundingList defaultBL(q);
	defaultBL.set();
  boundingChain = boundingchain_t(static_cast<unsigned long>(n), defaultBL);
}

/// A setter for vertex colour which respects bounding lists when
/// checkBoundingList == true \param v the vertex to update \param c the new
/// colour for v
void Model::setColour(int v, int c) {
  if ((not checkBoundingList) || getBoundingList(v)[c]) {
    colouring[v] = c;
  } else {
    throw std::invalid_argument(
        "New colour for vertex must be contained in bounding list.");
  }
}

/// Return the number of occurrences of each colour in the neighbours of a
/// vertex \param v the vertex to consider \return a vector describing the
/// number of occurrences of each colour in the neighbourhood
std::vector<int> Model::getNeighbourhoodColourCount(int v) const {
  std::vector<int> count(static_cast<unsigned long>(q));
  for (int neighbour : getNeighboursIndex(v)) {
    count[getColour(neighbour)]++;
  }
  return count;
}

/// Return the unfixed colours in the neighbourhood of v as a bitset
/// \param v the vertex whose neighbourhood to consider
/// \return	a bitset with set bits where a colour is in a bounding list of
/// size greater than one in the neighbourhood
BoundingList Model::bs_getUnfixedColours(int v) const {
  //	initialize result, noting that if a vertex has no neighbours then this
  // correctly defaults to all unset
  BoundingList result(q);
  BoundingList boundingList(q);

  for (int neighbour : getNeighboursIndex(v)) {
    boundingList = getBoundingList(neighbour);
    if (boundingList.count() > 1) {
      for (int i = 0; i < q; i++) {
        if (boundingList[i]) {
          result[i] = true;
        }
      }
    }
  }

  return result;
}

/// Return the unfixed colours in the neighbourhood
/// \sa bs_getUnfixedColours
/// \param v the vertex whose neighbourhood to consider
/// \return a vector listing the colours which are only in a bounding list of
/// size greater than one in the neighbourhood
std::vector<int> Model::getUnfixedColours(int v) const {
  return getIndexVector<BoundingList>(bs_getUnfixedColours(v));
}

/// Return the fixed colours in the neighbourhood of v as a bitset
/// \sa bs_getUnfixedColours
/// \param v the vertex whose neighbourhood to consider
/// \return	a bitset with set bits where a colour is only in bounding lists
/// of size one, or in no bounding lists in the neighbourhood
BoundingList Model::bs_getFixedColours(int v) const {
  BoundingList result = bs_getUnfixedColours(v);
  result.flip();
  return result;
}

/// Return the fixed colours in the neighbourhood
/// \sa bs_getFixedColours
/// \param v the vertex whose neighbourhood to consider
/// \return a vector listing the colours which only occur in bounding lists of
/// size one, or in no bounding lists in the neighbourhood
std::vector<int> Model::getFixedColours(int v) const {
  return getIndexVector<BoundingList>(bs_getFixedColours(v));
}

/// Return the fixed count of c in the neighbourhood v
/// \sa getFixedColours
/// \param v the vertex whose neighbourhood to consider
/// \param c the colour to consider
/// \return the number of occurrences of the colour c on the neighbours of v
/// where the bounding list on the neighbour also has size one
int Model::m_Q(int v, int c) const {
  int result = 0;
  BoundingList boundingList(q);

  for (int neighbour : getNeighboursIndex(v)) {
    boundingList = getBoundingList(neighbour);
    if (boundingList.count() == 1 && boundingList[c]) {
      result++;
    }
  }

  return result;
}

/// Setter for the bounding list at a vertex
/// \param v the vertex with the bounding list which is modified
/// \param boundingList the new bounding list for vertex v
void Model::setBoundingList(int v,
                            const std::vector<int> &boundingList) {
  boundingChain[v] = BoundingList{q, boundingList};
}

std::vector<BoundingList> Model::getBoundingLists(const std::vector<int> &vertices) const {
	std::vector<BoundingList> result(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		result[i] = getBoundingList(vertices[i]);
	}
	return result;
}

/// Return the `minimal' set A maximally intersecting the bounding lists of
/// greater neighbours of v \param v the vertex \param size the size of the
/// set A to return \return a bitset describing the set A
BoundingList Model::bs_generateA(int v, int size) const {
  std::vector<int> vertices = getNeighboursIndex(v);

  //	remove neighbours which are less than v
  vertices.erase(std::remove_if(vertices.begin(), vertices.end(),
                                [&v](int w) { return w < v; }),
                 vertices.end());

  BoundingList A = BoundingList::unionOfLists(getBoundingLists(vertices), q);

  //	ensure A has size at most size
  A.atMostKUp(size);

  //	add more colours if A is smaller than size
  for (int i = 0, count = static_cast<int>(A.count()); size - count > 0; count++) {
    while (A[i]) {
      i++;
    }
    A.flip(static_cast<unsigned long>(i));
  }

  return A;
}

/// helper function which creates a sampler object and samples from the
/// colours of the model
void Model::sample() {
  Sampler sampler(*this);
  sampler.sample();
}

/// constructor for the graph class
/// \param n the number of vertices in the graph
/// \param edges
Graph::Graph(int n, const std::__1::list<edge_t> &edges) {
  // Initialize matrix of size n x n
  adjacencyMatrix = adjmatrix_t(static_cast<unsigned long>(n), std::vector<bool>(static_cast<unsigned long>(n)));

  // Populate adjacencyMatrix with edges
  for (edge_t edge : edges) {
    adjacencyMatrix[edge.first][edge.second] = true;
    adjacencyMatrix[edge.second][edge.first] = true;
  }
}

Graph::Graph(int n, const std::string &type)
    : Graph(n, buildEdgeSet(n, type)) {}

/// helper function for constructing a set of edges
/// \param n the number of vertices in the graph
/// \param type the type of the graph (one of cycle, complete)
std::__1::list<edge_t> Graph::buildEdgeSet(int n, const std::string &type) {
	std::__1::list<edge_t> edges;
	if (type == "cycle") {
		for (int i = 0; i + 1 < n; i++) {
			edges.emplace_back(i, i + 1);
		}
		if (n > 2) {
			edges.emplace_back(n - 1, 0);
		}
	} else if (type == "complete") {
		int j;
		for (int i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				if (i != j) {
					edges.emplace_back(i, j);
				}
			}
		}
	} else {
		throw std::invalid_argument("Invalid graph type.");
	}
	return edges;
}

/// return the number of edges in the graph
int Graph::getEdgeCount() const {
  int total{0};
  for (std::vector<bool> neighbours : adjacencyMatrix) {
    total += std::count(neighbours.begin(), neighbours.end(), true);
  }
  return total / 2;
}