#include "update.hpp"

/*************************************
 * Helpers
 *************************************/

/// a method which applies the map c -> B^c to a vector
/// \param B the base B
/// \param counts a vector containing the exponent
/// \return a vector of weights B^c
std::vector<long double> pow(long double temperature, std::vector<int> counts) {
    std::vector<long double> weights(counts.size());
    std::transform(counts.begin(), counts.end(), weights.begin(),
                   [temperature](int m_c) { return pow(temperature, m_c); });
    return weights;
}

int sampleC2(const State &state, int v) {
    std::vector<long double> weights(state.parameters.maxColours);
    BoundingList bl = queries::getFixedColours(state.graph, state.parameters, state.boundingChain, v);
    for (int c{}; c < bl.size(); ++c) {
        if (bl[c]) {
            weights[c] = pow(state.parameters.temperature,
                             queries::m_Q(state.graph, state.parameters, state.boundingChain, v, c));
        }
    }

    return sampleFromDist<long double>(weights);
}

/*************************************
 * Contract Update
 *************************************/

/// constructor for Contract update which accepts c1
/// \param m the model to update
/// \param v the vertex to update
/// \param c1 the proposal for the new colour of v
ContractUpdate::ContractUpdate(const State &m, int v, int c1)
    : Update{m, v, c1},
      unfixedCount{
          static_cast<int>(queries::getUnfixedColours(state.graph, state.parameters, state.boundingChain, v).count())},
      c2{sampleC2(m, v)} {}

/// choose propose a new colour for the vertex v
/// \param m the model being updated
/// \param v the vertex to update
/// \return a new colour sampled uniformly from the set of unfixed colours at v
/// \sa Model::bs_getUnfixedColours
int ContractUpdate::proposeC1(const State &state, int v) {
    return uniformSample(queries::getUnfixedColours(state.graph, state.parameters, state.boundingChain, v));
}

/// compute the cutoff used to choose between c1 and c2
long double ContractUpdate::colouringGammaCutoff() const {
    std::vector<long double> weights =
        pow(state.parameters.temperature,
            queries::getNeighbourhoodColourCount(state.graph, state.parameters, state.colouring, v));
    long double norm = std::accumulate(weights.begin(), weights.end(), 0.0L);
    return pow(state.parameters.temperature, weights[c1]) * unfixedCount / norm;
}

/// compute the cutoff used to set the bounding chain
long double ContractUpdate::boundingListGammaCutoff() const {
    return unfixedCount /
           (state.parameters.maxColours - state.parameters.maxDegree * (1 - state.parameters.temperature));
}

/*************************************
 * Compress Update
 *************************************/

/// compute the cutoff used to choose between c1 and c2
/// \sa updateColouring
long double CompressUpdate::gammaCutoff() const {
    std::vector<long double> weights =
        pow(state.parameters.temperature,
            queries::getNeighbourhoodColourCount(state.graph, state.parameters, state.colouring, v));
    long double norm = std::accumulate(weights.begin(), weights.end(), 0.0L);
    return (state.parameters.maxColours - state.parameters.maxDegree) * weights[c1] / norm;
}

/// generate a sample from the set A
int CompressUpdate::sampleFromA() const {
    std::vector<long double> weights =
        pow(state.parameters.temperature,
            queries::getNeighbourhoodColourCount(state.graph, state.parameters, state.colouring, v));

    //	compute the denominator used to determine when to accept a colour as a sample
    long double norm = 0;
    for (int i = 0; i < weights.size(); i++) {
        if (A[i]) {
            norm += weights[i];
        }
    }

    long double tau_x_Denominator = tau * norm;
    long double total             = 0;
    for (int colour{}; colour < A.size(); ++colour) {
        if (!A[colour]) continue;

        if (total + weights[colour] > tau_x_Denominator) {
            return colour;
        }

        total += weights[colour];
    }

    throw std::runtime_error("No sample generated from A (likely caused by rounding error).");
}
