#include "model.hpp"

#include <boost/dynamic_bitset.hpp>

#include "sampler.hpp"

Model::Model(std::shared_ptr<const Parameters> parameters, Graph graph)
        : graph{std::move(graph)}, parameters{std::move(parameters)}, colouring{colouring_t(parameters->n)}
{
    BoundingList defaultBL(parameters->q);
    defaultBL.set();
    boundingChain = boundingchain_t(parameters->n, defaultBL);
}

/// A setter for vertex colour which respects bounding lists when
/// checkBoundingList == true \param v the vertex to update \param c the new
/// colour for v
void Model::setColour(int v, int c) {
    if (checkBoundingList && !getBoundingList(v)[c]) {
        throw std::invalid_argument("New colour for vertex must be contained in bounding list.");
    }
    colouring[v] = c;
}

/// Return the number of occurrences of each colour in the neighbours of a
/// vertex \param v the vertex to consider \return a vector describing the
/// number of occurrences of each colour in the neighbourhood
std::vector<int> Model::getNeighbourhoodColourCount(int v) const {
    std::vector<int> count(parameters->q);
    for (int neighbour : graph.getNeighboursIndex(v)) {
        ++count[getColour(neighbour)];
    }
    return count;
}

/// Return the unfixed colours in the neighbourhood of v as a bitset
/// \param v the vertex whose neighbourhood to consider
/// \return	a bitset with set bits where a colour is in a bounding list of
/// size greater than one in the neighbourhood
BoundingList Model::bs_getUnfixedColours(int v) const {
    // initialize result, noting that if a vertex has no neighbours then this
    // correctly defaults to all unset
    BoundingList result(parameters->q);
    BoundingList boundingList(parameters->q);

    for (int neighbour : graph.getNeighboursIndex(v)) {
        boundingList = getBoundingList(neighbour);
        if (boundingList.count() <= 1) {
            continue;
        }
        for (int i = 0; i < parameters->q; i++) {
            result[i] = boundingList[i];
        }
    }

    return result;
}

/// Return the unfixed colours in the neighbourhood
/// \sa bs_getUnfixedColours
/// \param v the vertex whose neighbourhood to consider
/// \return a vector listing the colours which are only in a bounding list of
/// size greater than one in the neighbourhood
std::vector<int> Model::getUnfixedColours(int v) const { return getIndexVector<BoundingList>(bs_getUnfixedColours(v)); }

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
std::vector<int> Model::getFixedColours(int v) const { return getIndexVector<BoundingList>(bs_getFixedColours(v)); }

/// Return the fixed count of c in the neighbourhood v
/// \sa getFixedColours
/// \param v the vertex whose neighbourhood to consider
/// \param c the colour to consider
/// \return the number of occurrences of the colour c on the neighbours of v
/// where the bounding list on the neighbour also has size one
int Model::m_Q(int v, int c) const {
    int result = 0;
    BoundingList boundingList(parameters->q);

    for (int neighbour : graph.getNeighboursIndex(v)) {
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
void Model::setBoundingList(int v, const std::vector<int> &boundingList) {
    boundingChain[v] = BoundingList{parameters->q, boundingList};
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
    std::vector<int> vertices = graph.getNeighboursIndex(v);

    //	remove neighbours which are less than v
    vertices.erase(
        std::remove_if(vertices.begin(), vertices.end(), [v](int w) { return w < v; }),
        vertices.end()
    );

    BoundingList A = BoundingList::unionOfLists(getBoundingLists(vertices), parameters->q);

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

