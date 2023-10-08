#include "state.hpp"

BoundingList::BoundingList(const int q, const std::vector<int> &boundingList) : boost::dynamic_bitset<>(q) {
    for (int colour : boundingList) {
        if (colour < 0 || colour >= q) {
            throw std::invalid_argument("Bounding list must be a subset of {0, ..., q - 1}");
        }

        set(colour);
    }
}

/// unset all bits after the kth set bit
/// \sa bs_generateA
/// \param k the number of bits set in after atMostKUp has been applied
/// \return a copy of bs with all set bits after the kth set bit unset
void BoundingList::atMostKUp(int k) {
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

/// Return the union of the bounding lists at all the vertices
/// \param vertices
/// \return the bitwise OR of the bounding list bitsets
BoundingList BoundingList::unionOfLists(const std::vector<BoundingList> &lists, int q) {
    BoundingList result{q};
    for (const BoundingList &bl : lists) {
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

/// constructor for the graph class
/// \param n the number of vertices in the graph
/// \param edges
Graph::Graph(int n, const std::vector<edge_t> &edges) {
    // Initialize matrix of size n x n
    adjacencyMatrix = std::vector<std::vector<bool>>(n, std::vector<bool>(n));

    // Populate adjacencyMatrix with edges
    for (const edge_t& edge : edges) {
        adjacencyMatrix[edge.first][edge.second] = true;
        adjacencyMatrix[edge.second][edge.first] = true;
    }
}

/// helper function for constructing a set of edges
/// \param n the number of vertices in the graph
/// \param type the type of the graph (one of cycle, complete)
std::vector<Graph::edge_t> Graph::buildEdgeSet(int n, const std::string &type) {
    std::vector<edge_t> edges;
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
int Graph::numEdges() const {
    int total{0};
    for (const std::vector<bool> &neighbours : adjacencyMatrix) {
        total += std::count(neighbours.begin(), neighbours.end(), true);
    }
    return total / 2;
}

