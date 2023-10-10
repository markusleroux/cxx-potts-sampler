#include "sampler.hpp"

#include "update.hpp"

/*************************************
 * Parameters
 *************************************/

bool Parameters::verify(const Graph& graph) const {
    bool failed = false;

    if (graph.getMaxDegree() < 3) {
        std::cout << "Delta must be greater than 2." << std::endl;
        failed |= true;
    }

    if (maxColours <= 2 * graph.getMaxDegree()) {
        std::cout << "The number of colours q must be greater than 2 * Delta. Delta is " << graph.getMaxDegree() << '.' << std::endl;
        failed |= true;
    }

    if (temperature >= 1) {
        std::cout << "B must be less than 1." << std::endl;
        failed |= true;
    }

    if (const long double min_B = 1 - static_cast<long double>(maxColours - 2 * graph.getMaxDegree()) / graph.getMaxDegree();
        temperature <= min_B) {
        std::cout << "B, Delta and q must satisfy B > 1 - (q - 2 * Delta) / Delta, i.e. B > " << std::to_string(min_B)
                  << std::endl;
        failed |= true;
    }

    return !failed;
}

/*************************************
 * Graph
 *************************************/

Graph::Graph(int numNodes, const std::vector<edge_t> &edges)
        : adjacencyMatrix{std::vector<std::vector<int>>(numNodes, std::vector<int>())}
{
    for (const edge_t &edge : edges) {
        adjacencyMatrix[edge.first].emplace_back(edge.second);
        adjacencyMatrix[edge.second].emplace_back(edge.first);
    }

    for (int node{}; node < adjacencyMatrix.size(); ++node) {
        maxDegree = std::max(maxDegree, static_cast<int>(adjacencyMatrix[node].size()));
    }
}

/// helper function for constructing a set of edges
/// \param n the number of vertices in the graph
/// \param type the type of the graph (one of cycle, complete)
std::vector<Graph::edge_t> Graph::buildEdgeSet(int n, Graph::Type type) {
    std::vector<edge_t> edges;

    switch (type) {
        case Type::CYCLE:
            for (int i = 0; i + 1 < n; i++) {
                edges.emplace_back(i, i + 1);
            }
            if (n > 2) {
                edges.emplace_back(n - 1, 0);
            }
            return edges;

        case Type::COMPLETE:
            int j;
            for (int i = 0; i < n; i++) {
                for (j = 0; j < i; j++) {
                    if (i != j) {
                        edges.emplace_back(i, j);
                    }
                }
            }
            return edges;

        default:
            throw std::invalid_argument("Invalid graph type.");
    }
}

int Graph::numEdges() const {
    return std::accumulate(adjacencyMatrix.begin(), adjacencyMatrix.end(), 0,
                           [](int acc, const auto &neighbours) { return acc + neighbours.size(); }) /
           2;
}

std::ostream &operator<<(std::ostream &out, const Graph &graph) {
    for (int v = 0; v < graph.size(); v++) {
        out << v << ": {";
        for (auto n : graph.getNeighbours(v)) {
            out << "}" << std::endl;
        }
    }
    return out;
}

std::istream& operator>>(std::istream& is, Graph::Type& type) {
    std::string token;
    is >> token;
    if (token == "cycle") {
        type = Graph::Type::CYCLE;
    } else if (token == "complete") {
        type = Graph::Type::COMPLETE;
    } else {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}


/*************************************
 * Main Sampling Algorithm
 *************************************/

struct Epoch {
    std::vector<CompressUpdate> phaseOneHistory{};
    std::vector<ContractUpdate> phaseTwoHistory{};
};

static int getPhaseTwoIters(const Graph &graph, const Parameters &parameters);

void update(State &state, const ContractUpdate &update);
void update(State &state, const CompressUpdate &update);

void updateColouring(State &state, const ContractUpdate &update);
void updateColouring(State &state, const CompressUpdate &update);

void updateColourWithEpoch(State &model, Epoch &epoch);

Epoch epoch(State &model, int phaseTwoIters);

void sample(State &state);


std::optional<std::vector<int>> sample(const Parameters &parameters, const Graph &graph) {
    if (!parameters.verify(graph)) {
        return std::nullopt;
    }

    BoundingList defaultBL(parameters.maxColours);
    defaultBL.set();

    State state{.parameters    = parameters,
                .graph         = graph,
                .colouring     = colouring_t(parameters.numNodes),
                .boundingChain = boundingchain_t(parameters.numNodes, defaultBL)};
    sample(state);
    return {state.colouring};
}

void sample(State &state) {
    int phaseTwoIters = getPhaseTwoIters(state.graph, state.parameters);
    std::vector<Epoch> history;

    // iterate until boundingChainIsConstant holds
    int t;
    for (t = 0; !queries::boundingChainIsConstant(state.boundingChain); t++) {
        history.emplace_back(epoch(state, phaseTwoIters));
    }

    // apply history (reversed)
    t = 0;
    for (auto it = ++history.rbegin(); it != history.rend(); it++, t++) {
        updateColourWithEpoch(state, *it);
    }
}

/// run a single epoch of the algorithm
Epoch epoch(State &state, int phaseTwoIters) {
    Epoch epoch;

    // Phase One
    BoundingList A(state.parameters.maxColours);
    for (int v = 0; v < state.graph.size(); v++) {
        // set A for the neighbourhood of v
        A = queries::getA(state.graph, state.parameters, state.boundingChain, v, state.graph.getMaxDegree());
        for (int w : state.graph.getNeighbours(v)) {
            if (w > v) {
                epoch.phaseOneHistory.emplace_back(state, w, A);
                update(state, epoch.phaseOneHistory.back());
            }
        }

        epoch.phaseTwoHistory.emplace_back(state, v);
        update(state, epoch.phaseTwoHistory.back());
    }

    // Phase Two
    int v;
    for (int i = 0; i < phaseTwoIters; i++) {
        // choose v uniformly at random
        BoundingList bl(state.graph.size());
        bl.flip();
        v = uniformSample(bl);
        epoch.phaseTwoHistory.emplace_back(state, v);
        update(state, epoch.phaseTwoHistory.back());
    }

    return epoch;
}

static int getPhaseTwoIters(const Graph &graph, const Parameters &parameters) {
    return graph.size() + 1 + graph.numEdges() +
           pow(graph.size(), 2) * (parameters.maxColours -
                                   graph.getMaxDegree() * (1 - parameters.temperature) /
                                       (parameters.maxColours - graph.getMaxDegree() * (3 - parameters.temperature)));
}

// TODO: concept would be useful to remove this duplication
void update(State &state, const CompressUpdate &update) {
    state.boundingChain[update.v] =
        update.getNewBoundingChain();  // bounding chain must be updated before the colouring
    updateColouring(state, update);
}

void update(State &state, const ContractUpdate &update) {
    state.boundingChain[update.v] =
        update.getNewBoundingChain();  // bounding chain must be updated before the colouring
    updateColouring(state, update);
}

// TODO: concept would be useful to remove this duplication
void updateColouring(State &state, const CompressUpdate &update) {
    try {
        state.colouring[update.v] = update.getNewColour();
    } catch (const std::runtime_error &err) {
        throw std::runtime_error("Compress update (vertex + " + std::to_string(update.v) +
                                 ") failed with exception: " + err.what());
    } catch (const std::invalid_argument &err) {
        throw std::runtime_error("Compress updated (vertex + " + std::to_string(update.v) +
                                 ") failed with invalid_argument:" + err.what());
    }
}

void updateColouring(State &state, const ContractUpdate &update) {
    try {
        state.colouring[update.v] = update.getNewColour();
    } catch (const std::runtime_error &err) {
        throw std::runtime_error("Contract update (vertex + " + std::to_string(update.v) +
                                 ") failed with exception: " + err.what());
    } catch (const std::invalid_argument &err) {
        throw std::runtime_error("Contract updated (vertex + " + std::to_string(update.v) +
                                 ") failed with invalid_argument:" + err.what());
    }
}

void updateColourWithEpoch(State &state, Epoch &epoch) {
    for (auto &iteration : epoch.phaseOneHistory) {
        updateColouring(state, iteration);
    }
    for (auto &iteration : epoch.phaseTwoHistory) {
        updateColouring(state, iteration);
    }
}
