#include "Model.h"
#include "Sampler.h"
#include <algorithm>

#define q 10


std::mt19937 Model::mersene_gen{std::random_device{}()};

// Define model constructor
Model::Model(unsigned int n, const std::list<std::pair<unsigned, unsigned>>& edges) : Graph(n, edges) {
    // Initialize vector of colours
    colouring = std::vector<unsigned int>(n);

    // Initialize bounding chain
    boundingChain = std::vector< boost::dynamic_bitset<> >(n, ~boost::dynamic_bitset<>(q));
}

void Model::setColour(unsigned int v, unsigned int c) {
    if ((not checkBoundingList) || getBoundingList(v)[c]) {
        colouring[v] = c;
    } else {
        throw std::invalid_argument("New colour for vertex must be contained in bounding list.");
    }
}

std::vector<unsigned int> Model::getNeighbourhoodColourCount(unsigned int v) const {
    std::vector<unsigned int> count(q);
    for (int neighbour : getNeighboursIndex(v)) {
        count[getColour(neighbour)]++;
    }
    return count;
}

boost::dynamic_bitset<> Model::bs_getUnfixedColours(unsigned int v) const {
    boost::dynamic_bitset<> result(q);
    boost::dynamic_bitset<> boundingList(q);

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

std::vector<unsigned int> Model::getUnfixedColours(unsigned int v) const {
    return getIndexVector<boost::dynamic_bitset<>>(bs_getUnfixedColours(v));
}

boost::dynamic_bitset<> Model::bs_getFixedColours(unsigned int v) const {
    boost::dynamic_bitset<> notResult = bs_getUnfixedColours(v);
    return ~notResult;
}

std::vector<unsigned int> Model::getFixedColours(unsigned int v) const {
    return getIndexVector<boost::dynamic_bitset<>>(bs_getFixedColours(v));
}

unsigned int Model::m_Q(unsigned int v, unsigned int c) const {
    int result = 0;
    boost::dynamic_bitset<> boundingList(q);

    for (int neighbour : getNeighboursIndex(v)) {
        boundingList = getBoundingList(neighbour);
        if (boundingList.count() == 1 && boundingList[c]) { result++; }
    }

    return result;
}

unsigned int Model::m(unsigned int v, unsigned int c) const { return getNeighbourhoodColourCount(v)[c]; }

void Model::setBoundingList(unsigned int v, const std::vector<unsigned int> &boundingList) {
    boost::dynamic_bitset<> bs_boundingList(q);
    for (unsigned colour : boundingList) {
        if (colour >= 0 && colour < q) {
            bs_boundingList[colour] = true;
        } else {
            throw std::invalid_argument("Bounding list must be a subset of {0, ..., q - 1}");
        }
    }

    boundingChain[v] = bs_boundingList;
}

boost::dynamic_bitset<> Model::UnionOfBoundingLists(const std::vector<unsigned int> &vertices) const {
    boost::dynamic_bitset<> result(q);

    // Iterate through the bounding lists of neighbours of v
    for (unsigned neighbour : vertices) {
        result |= getBoundingList(neighbour);
    }

    return result;
}

boost::dynamic_bitset<> Model::atMostKUp(boost::dynamic_bitset<> bs, unsigned int k) {
    for (int i = 0; i < bs.size(); i++) {
        if (bs[i] && k == 0) {
            bs[i] = false;
        } else if (bs[i]) { k--; }
    }

    return bs;
}

boost::dynamic_bitset<> Model::bs_generateA(unsigned int v, unsigned int size) const {
    assert(size <= q);

    std::vector<unsigned> vertices = getNeighboursIndex(v);
    vertices.erase(std::remove_if(vertices.begin(),
                                       vertices.end(),
                                       [&v](int w){ return w < v; }),
                   vertices.end());

    boost::dynamic_bitset<> A = atMostKUp(UnionOfBoundingLists(vertices), size);
    for (int i = 0, count = A.count(); size - count > 0; count++) {
        while (A[i]) { i++; }
        A.flip(i);
    }

    return A;
}

void Model::sample() {
	Sampler sampler(*this);
	sampler.sample();
}