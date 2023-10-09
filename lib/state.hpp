#ifndef POTTSSAMPLER_STATE_H
#define POTTSSAMPLER_STATE_H

#include <boost/dynamic_bitset.hpp>
#include <memory>
#include <random>
#include <vector>

#include "sampler.hpp"

struct BoundingList : boost::dynamic_bitset<> {
    explicit BoundingList(const int maxColours) : boost::dynamic_bitset<>(maxColours) {}

    BoundingList(int maxColours, const std::vector<int> &boundingList);

    void makeAtMostKSet(int k);
    BoundingList flip_copy() const;
};

using boundingchain_t = std::vector<BoundingList>;

struct State {
    const Parameters parameters;
    const Graph graph;

    colouring_t colouring;
    boundingchain_t boundingChain;
};

namespace queries {
BoundingList getUnfixedColours(const Graph &, const Parameters &, const boundingchain_t &, int v);
BoundingList getFixedColours(const Graph &, const Parameters &, const boundingchain_t &, int v);
std::vector<int> getNeighbourhoodColourCount(const Graph &, const Parameters &, const colouring_t &, int v);

/// Return the `minimal' set A maximally intersecting the bounding lists of
/// greater neighbours of v \param v the vertex \param size the size of the
/// set A to return \return a bitset describing the set A
BoundingList getA(const Graph &, const Parameters &, const boundingchain_t &, int v, int size);

bool boundingChainIsConstant(const boundingchain_t &);

/// Return the fixed count of c in the neighbourhood v
/// \sa getFixedColours
/// \param v the vertex whose neighbourhood to consider
/// \param c the colour to consider
/// \return the number of occurrences of the colour c on the neighbours of v
/// where the bounding list on the neighbour also has size one
int m_Q(const Graph &, const Parameters &, const boundingchain_t &, int v, int c);
}  // namespace queries

#endif
