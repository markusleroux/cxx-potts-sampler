#include "state.hpp"

/*************************************
 * Bounding List
 *************************************/

BoundingList::BoundingList(const int maxColours, const std::vector<int>& boundingList)
    : boost::dynamic_bitset<>(maxColours) {
    for (int colour : boundingList) {
        if (colour < 0 || colour >= maxColours) {
            throw std::invalid_argument("Bounding list must be a subset of {0, ..., q - 1}");
        }

        set(colour);
    }
}

/// unset all bits after the kth set bit
/// \sa bs_generateA
/// \param k the number of bits set in after atMostKUp has been applied
/// \return a copy of bs with all set bits after the kth set bit unset
void BoundingList::makeAtMostKSet(int k) {
    for (int i = 0; i < size(); i++) {
        if (!test(i)) {
            continue;
        }

        if (k == 0) {
            reset(i);
        } else {
            k--;
        }
    }
}

// \brief analogous to flip but not in place
BoundingList BoundingList::flip_copy() const {
    BoundingList result{*this};
    result.flip();
    return result;
}

bool boundingChainIsConstant(const boundingchain_t& boundingChain) {
    return std::all_of(boundingChain.begin(), boundingChain.end(),
                       [](const BoundingList& bs) { return bs.count() == 1; });
}

namespace queries {
BoundingList getUnfixedColours(const Graph& graph, const Parameters& parameters, const boundingchain_t& boundingChain,
                               int v) {
    // initialize result, noting that if a vertex has no neighbours then this
    // correctly defaults to all unset
    BoundingList result(parameters.maxColours);
    BoundingList boundingList(parameters.maxColours);

    for (int neighbour : graph.getNeighbours(v)) {
        boundingList = boundingChain[neighbour];
        if (boundingList.count() <= 1) {
            continue;
        }
        for (int colour = 0; colour < parameters.maxColours; colour++) {
            result[colour] = boundingList[colour];
        }
    }

    return result;
}

BoundingList getFixedColours(const Graph& graph, const Parameters& parameters, const boundingchain_t& boundingChain,
                             int v) {
    auto fixedColours = getUnfixedColours(graph, parameters, boundingChain, v);
    fixedColours.flip();
    return fixedColours;
}

std::vector<int> getNeighbourhoodColourCount(const Graph& graph, const Parameters& parameters,
                                             const colouring_t& colouring, int v) {
    std::vector<int> count(parameters.maxColours);
    for (int neighbour : graph.getNeighbours(v)) {
        ++count[colouring[neighbour]];
    }
    return count;
}

BoundingList getA(const Graph& graph, const Parameters& parameters, const boundingchain_t& boundingChain, int v,
                  int size) {
    std::vector<int> vertices = graph.getNeighbours(v);

    //	remove neighbours which are less than v
    vertices.erase(std::remove_if(vertices.begin(), vertices.end(), [v](int w) { return w < v; }), vertices.end());

    BoundingList A{parameters.maxColours};
    for (int vertex : vertices) {
        A |= boundingChain[vertex];
    }

    //	ensure A has size at most size
    A.makeAtMostKSet(size);

    //	add more colours if A is smaller than size
    for (int colour = 0, count = static_cast<int>(A.count()); size - count > 0; count++) {
        while (A[colour]) {
            colour++;
        }
        A.flip(colour);
    }

    return A;
}

int m_Q(const Graph& graph, const Parameters& parameters, const boundingchain_t& boundingChain, int v, int c) {
    int Q = 0;
    BoundingList boundingList(parameters.maxColours);

    for (int neighbour : graph.getNeighbours(v)) {
        boundingList = boundingChain[neighbour];
        if (boundingList.count() == 1 && boundingList[c]) {
            Q++;
        }
    }

    return Q;
}
}  // namespace queries
