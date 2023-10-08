#include <boost/dynamic_bitset.hpp>
#include <memory>
#include <random>
#include <vector>


struct Parameters {
    int n;
    int q;
    int Delta;
    long double B;
};

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
    explicit BoundingList(const int q) : boost::dynamic_bitset<>(q) {}
    BoundingList(int q, const std::vector<int> &boundingList);

    void atMostKUp(int k);
    static BoundingList unionOfLists(const std::vector<BoundingList> &lists, int q);

    BoundingList C() const;
};

using boundingchain_t = std::vector<BoundingList>;


class Graph {
   public:
    using edge_t = std::pair<int, int>;

    Graph(int n, const std::string &type) : Graph(n, buildEdgeSet(n, type)) {}
    Graph(int n, const std::vector<edge_t> &edges);

    int size() const { return static_cast<int>(adjacencyMatrix.size()); }
    int numEdges() const;

    /// getter for column v of the adjacency matrix
    const std::vector<bool>& getNeighboursBool(int v) const { return adjacencyMatrix[v]; }

    /// a method which gets the indices of the neighbours of v
    std::vector<int> getNeighboursIndex(int v) const {
        return getIndexVector<std::vector<bool>>(getNeighboursBool((v)));
    }

    friend std::ostream &operator<<(std::ostream &out, const Graph &graph) {
        for (int v = 0; v < graph.size(); v++) {
            out << v << ": {";
            for (auto n : graph.getNeighboursIndex(v)) {
                out << "}\n";
            }
        }
        return out;
    }

   private:
    static std::vector<edge_t> buildEdgeSet(int n, const std::string &type);

    std::vector<std::vector<bool>> adjacencyMatrix;
};



using colouring_t = std::vector<int>;

struct State {
    const Graph graph;
    const Parameters parameters;

    colouring_t colouring;
    boundingchain_t boundingChain;
};

