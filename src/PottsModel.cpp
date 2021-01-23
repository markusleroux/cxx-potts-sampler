#include "PottsModel.h"
#include <algorithm>

#define q 10

Graph::Graph(unsigned n, std::list<std::pair<unsigned, unsigned>> edges) {
    // Initialize matrix of size n x n
    std::vector< std::vector<bool> > adjacencyMatrix(n, std::vector<bool>(n));

    // Populate adjacencyMatrix with edges
    for (std::pair<unsigned, unsigned> edge : edges) {
        adjacencyMatrix[edge.first][edge.second] = 1;
        adjacencyMatrix[edge.second][edge.first] = 1;
    }

    // Initialize vector of colours
    std::vector<int> colouring(n);

    // Initialize bounding chain
    std::vector< boost::dynamic_bitset<> > boundingChain(n, ~boost::dynamic_bitset<>(q));
}

void Graph::setColour(int v, int c) {
    if (boundingChain[v][c]) {
        colouring[v] = c;
    } else {
        throw std::invalid_argument("New colour for vertex must be contained in bounding list.");
    }
}

template <typename bitset_t>
std::vector<int> Graph::getIndexVector(bitset_t bs) {
    std::vector<int> result;
    for (int i = 0; i < bs.size(); i++) {
        if (bs[i]) { result.push_back(i); }
    }
    return result;
}

std::vector<int> Graph::getNeighbourhoodColourCount(int v) {
    std::vector<int> count(q);
    for (int neighbour : getNeighboursIndex(v)) {
        count[getColour(neighbour)]++;
    }
    return count;
}

boost::dynamic_bitset<> Graph::bs_getUnfixedColours(int v) {
    boost::dynamic_bitset<> result(q);
    boost::dynamic_bitset<> boundingList(q);

    for (int neighbour : getNeighboursIndex(v)) {
        boundingList = getBoundingList(neighbour);
        if (boundingList.count() > 1) {
            for (int i = 0; i < q; i++) {
                if (boundingList[i]) {
                    result[i] = 1;
                }
            }
        }
    }

    return result;
}

std::vector<int> Graph::getUnfixedColours(int v) {
    return getIndexVector<boost::dynamic_bitset<>>(bs_getUnfixedColours(v));
}

boost::dynamic_bitset<> Graph::bs_getFixedColours(int v) {
    boost::dynamic_bitset<> notResult = bs_getUnfixedColours(v);
    return ~notResult;
}

std::vector<int> Graph::getFixedColours(int v) {
    return getIndexVector<boost::dynamic_bitset<>>(bs_getFixedColours(v));
}

int Graph::m_Q(int v, int c) {
    int result = 0;
    boost::dynamic_bitset<> boundingList(q);

    for (int neighbour : getNeighboursIndex(v)) {
        boundingList = getBoundingList(neighbour);
        if (boundingList.count() == 1 && boundingList[c]) { result++; }
    }

    return result;
}

int Graph::m(int v, int c) { return getNeighbourhoodColourCount(v)[c]; }

void Graph::setBoundingList(int v, std::vector<int> boundingListColours) {
    boost::dynamic_bitset<> boundingList(q);
    for (int colour : boundingListColours) {
        if (colour >= 0 && colour < q) {
            boundingList[colour] = 1;
        } else {
            throw std::invalid_argument("Bounding list must be a subset of {0, ..., q - 1}");
        }
    }

    boundingChain[v] = boundingList;
}

boost::dynamic_bitset<> Graph::UnionOfBoundingLists(int v) {
    boost::dynamic_bitset<> result(q);

    // Iterate through the bounding lists of neighbours of v
    for (int neighbour : getNeighboursIndex(v)) {
        result |= getBoundingList(neighbour);
    }

    return result;
}

boost::dynamic_bitset<> Graph::atMostKUp(boost::dynamic_bitset<> bs, int k) {
    for (int i = 0; i < bs.size(); i++) {
        if (bs[i] && k == 0) {
            bs[i] = 0;
        } else if (bs[i]) { k--; }
    }

    return bs;
}

boost::dynamic_bitset<> Graph::bs_generateA(int v, int size, std::vector<int> vertices) {
    assert(size <= q);

    boost::dynamic_bitset<> A = atMostKUp(UnionOfBoundingLists(v), size);
    for (int i = 0, count = A.count(); size - count > 0; count++) {
        while (A[i]) { i++; }
        A.flip(i);
    }

    return A;
}