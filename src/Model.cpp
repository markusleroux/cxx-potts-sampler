#include "Model.h"
#include "Sampler.h"
#include <algorithm>

BoundingList::BoundingList(const unsigned q,
                           const std::vector<unsigned> &boundingList)
    : boost::dynamic_bitset<>(q) {
  for (unsigned colour : boundingList) {
    if (colour >= 0 && colour < q) {
      set(colour);
    } else {
      throw std::invalid_argument(
          "Bounding list must be a subset of {0, ..., q - 1}");
    }
  }
}

BoundingList::BoundingList(const unsigned q) : boost::dynamic_bitset<>(q) {}

/// unset all bits after the kth set bit
/// \sa bs_generateA
/// \param k the number of bits set in after atMostKUp has been applied
/// \return a copy of bs with all set bits after the kth set bit unset
void BoundingList::atMostKUp(unsigned int k) {
  for (int i = 0; i < size(); i++) {
    if (test(i) && k == 0) {
      reset(i);
    } else if (test(i)) {
      k--;
    }
  }
}

BoundingList
BoundingList::unionOfLists(const std::vector<BoundingList> &lists) {
  BoundingList result{static_cast<unsigned int>(lists[0].size())};      // q won't be so large
  for (const BoundingList& bl : lists) {
    result |= bl;
  }
  return result;
}

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
Model::Model(unsigned int n, unsigned int q, unsigned int Delta, long double B,
             const std::list<edge_t> &edges)
    : Graph(n, edges), q(q), Delta(Delta), B(B) {
  // Initialize vector of colours
  colouring = std::vector<unsigned>(n);

	// Initialize bounding chain
	BoundingList defaultBL(q);
	defaultBL.set();
	boundingChain = boundingchain_t(n, defaultBL);
}

/// constructor for some generic graphs
/// \param type type of graph, may be one of {"cycle", "complete"}
/// \param n the number of vertices in the graph
/// \param q the number of colours
/// \param Delta the maximum degree of the graph
/// \param B the strength of the interactions between vertices
/// \return a model object
Model::Model(unsigned int n, unsigned int q, unsigned int Delta, long double B,
             const std::string &type)
    : Graph(n, type), q(q), Delta(Delta), B(B) {
  // Initialize vector of colours
  colouring = colouring_t(n);

  // Initialize bounding chain
	BoundingList defaultBL(q);
	defaultBL.set();
  boundingChain = boundingchain_t(n, defaultBL);
}

/// A setter for vertex colour which respects bounding lists when
/// checkBoundingList == true \param v the vertex to update \param c the new
/// colour for v
void Model::setColour(unsigned int v, unsigned int c) {
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
std::vector<unsigned int> Model::getNeighbourhoodColourCount(unsigned v) const {
  std::vector<unsigned> count(q);
  for (int neighbour : getNeighboursIndex(v)) {
    count[getColour(neighbour)]++;
  }
  return count;
}

/// Return the unfixed colours in the neighbourhood of v as a bitset
/// \param v the vertex whose neighbourhood to consider
/// \return	a bitset with set bits where a colour is in a bounding list of
/// size greater than one in the neighbourhood
BoundingList Model::bs_getUnfixedColours(unsigned int v) const {
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
std::vector<unsigned int> Model::getUnfixedColours(unsigned int v) const {
  return getIndexVector<BoundingList>(bs_getUnfixedColours(v));
}

/// Return the fixed colours in the neighbourhood of v as a bitset
/// \sa bs_getUnfixedColours
/// \param v the vertex whose neighbourhood to consider
/// \return	a bitset with set bits where a colour is only in bounding lists
/// of size one, or in no bounding lists in the neighbourhood
BoundingList Model::bs_getFixedColours(unsigned int v) const {
  BoundingList result = bs_getUnfixedColours(v);
  result.flip();
  return result;
}

/// Return the fixed colours in the neighbourhood
/// \sa bs_getFixedColours
/// \param v the vertex whose neighbourhood to consider
/// \return a vector listing the colours which only occur in bounding lists of
/// size one, or in no bounding lists in the neighbourhood
std::vector<unsigned int> Model::getFixedColours(unsigned int v) const {
  return getIndexVector<BoundingList>(bs_getFixedColours(v));
}

/// Return the fixed count of c in the neighbourhood v
/// \sa getFixedColours
/// \param v the vertex whose neighbourhood to consider
/// \param c the colour to consider
/// \return the number of occurrences of the colour c on the neighbours of v
/// where the bounding list on the neighbour also has size one
unsigned int Model::m_Q(unsigned int v, unsigned int c) const {
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
void Model::setBoundingList(unsigned int v,
                            const std::vector<unsigned int> &boundingList) {
  boundingChain[v] = BoundingList{q, boundingList};
}

/// Return the union of the bounding lists at all the vertices
/// \param vertices
/// \return the bitwise OR of the bounding list bitsets
BoundingList
Model::UnionOfBoundingLists(const std::vector<unsigned int> &vertices) const {
  BoundingList result(q);
  for (unsigned w : vertices) {
    result |= getBoundingList(w);
  }

  return result;
}

/// Return the `minimal' set A maximally intersecting the bounding lists of
/// greater neighbours of v \param v the vertex \param size the size of the
/// set A to return \return a bitset describing the set A
BoundingList Model::bs_generateA(unsigned int v, unsigned int size) const {
  std::vector<unsigned> vertices = getNeighboursIndex(v);

  //	remove neighbours which are less than v
  vertices.erase(std::remove_if(vertices.begin(), vertices.end(),
                                [&v](int w) { return w < v; }),
                 vertices.end());

  BoundingList A = UnionOfBoundingLists(vertices);

  //	ensure A has size at most size
  A.atMostKUp(size);

  //	add more colours if A is smaller than size
  for (int i = 0, count = A.count(); size - count > 0; count++) {
    while (A[i]) {
      i++;
    }
    A.flip(i);
  }

  return A;
}

/// helper function which creates a sampler object and samples from the
/// colours of the model
void Model::sample() {
  Sampler sampler(*this);
  sampler.sample();
}
