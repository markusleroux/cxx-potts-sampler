#ifndef POTTSSAMPLER_MODEL_H
#define POTTSSAMPLER_MODEL_H

#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

/// template which returns the indices of the set bits
/// \tparam bitset_t the type of the bitset
/// \param bs
/// \return the indices of set bits in bs
template<typename bitset_t>
static std::vector<int> getIndexVector(const bitset_t &bs) {
    std::vector<int> result;
    for (int i = 0; i < bs.size(); ++i) {
        if (bs[i]) {
            result.push_back(i);
        }
    }
    return result;
}

struct BoundingList : boost::dynamic_bitset<> {
    BoundingList() = default;

    explicit BoundingList(const int q) : boost::dynamic_bitset<>(static_cast<unsigned long>(q)) {}

    BoundingList(int q, const std::vector<int> &boundingList);

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
   public:
    Graph(int n, const std::vector<edge_t> &edges);
    Graph(int n, const std::string &type);

    /// get the number of vertices in the graph
    int getSize() const { return static_cast<int>(adjacencyMatrix.size()); }

    int getEdgeCount() const;

    /// getter for column v of the adjacency matrix
    std::vector<bool> getNeighboursBool(int v) const { return adjacencyMatrix[v]; }

    /// a method which gets the indices of the neighbours of v
    std::vector<int> getNeighboursIndex(int v) const {
        return getIndexVector<std::vector<bool>>(getNeighboursBool((v)));
    }

    friend std::ostream &operator<<(std::ostream &out, const Graph &graph) {
        for (int v = 0; v < graph.getSize(); v++) {
            out << v << ": {";
            for (auto n : graph.getNeighboursIndex(v)) {
                out << "}\n";
            }
        }
        return out;
    }

   private:
    static std::vector<edge_t> buildEdgeSet(int n, const std::string &type);

    adjmatrix_t adjacencyMatrix;
};

struct Parameters {
    int n;
    int q;
    int Delta;
    long double B;
};

/// \brief a class for holding parameters of the model, the colourings and the
/// bounding chain
class Model {
   public:
    Model(std::shared_ptr<const Parameters> parameters, const std::vector<edge_t> &edges);
    Model(std::shared_ptr<const Parameters> parameters, const std::string &type);

    /// get the colour of a specific vertex
    int getColour(int v) const { return colouring[v]; }

    /// get the current colouring of the graph
    const colouring_t &getColouring() { return colouring; }

    std::vector<int> getUnfixedColours(int v) const;
    std::vector<int> getFixedColours(int v) const;
    std::vector<int> getNeighbourhoodColourCount(int v) const;

    /// getter for the bounding chain
    const boundingchain_t &getBoundingChain() const { return boundingChain; }

    std::vector<BoundingList> getBoundingLists(const std::vector<int> &vertices) const;

    /// getter for the bounding list at vertex v
    BoundingList getBoundingList(int v) const { return boundingChain[v]; }

    std::vector<int> getBoundingListIndex(int v) const { return getIndexVector(getBoundingList(v)); }

    void sample();

    friend std::ostream &operator<<(std::ostream &out, const Model &model) { return out << model.graph; }

    const Graph graph;
    const std::shared_ptr<const Parameters> parameters;

   protected:
    colouring_t colouring;
    boundingchain_t boundingChain;

    bool checkBoundingList = true;

    BoundingList bs_getUnfixedColours(int v) const;
    BoundingList bs_getFixedColours(int v) const;

    int m_Q(int v, int c) const;
    void setColour(int v, int c);

    /// setter for the checkBoundingList property
    void setBoundingListChecks(bool b) { checkBoundingList = b; }

    /// setter for bounding list
    void setBoundingList(int v, const std::vector<int> &boundingList);
    BoundingList bs_generateA(int v, int size) const;

    friend class CompressUpdate;
    friend class ContractUpdate;
    friend class Sampler;
};

template<typename element_type>
std::ostream &operator<<(std::ostream &out, const std::vector<element_type> &vector) {
    if (vector.empty()) {
        return out;
    }

    for (auto it = std::begin(vector); it != std::prev(std::end(vector)); it++) {
        out << *it << ",";
    }
    out << vector.back();
    return out;
}

#endif  // POTTSSAMPLER_MODEL_H
