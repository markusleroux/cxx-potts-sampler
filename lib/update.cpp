#include "update.hpp"

/// a method which applies the map c -> B^c to a vector
/// \param B the base B
/// \param counts a vector containing the exponent
/// \return a vector of weights B^c
std::vector<long double> computeWeights(long double B, std::vector<int> counts) {
    std::vector<long double> weights(counts.size());
    std::transform(counts.begin(), counts.end(), weights.begin(), [B](int m_c) { return pow(B, m_c); });
    return weights;
}

/// constructor for Contract update which chooses c1
/// \param model the model to update
/// \param v the vertex to update
ContractUpdate::ContractUpdate(Model &model, int v) : ContractUpdate(model, v, handle_c1(model, v)) {}

/// constructor for Contract update which accepts c1
/// \param m the model to update
/// \param v the vertex to update
/// \param c1 the proposal for the new colour of v
ContractUpdate::ContractUpdate(Model &m, int v, int c1)
        : Update(m, v, c1), unfixedCount{static_cast<int>(model.bs_getUnfixedColours(v).count())}
{
    std::vector<long double> weights(model.parameters->q);
    for (int c : model.getFixedColours(v)) {
        weights[c] = pow(model.parameters->B, model.m_Q(v, c));
    }

    c2 = sampleFromDist<long double>(weights);
}

/// choose propose a new colour for the vertex v
/// \param m the model being updated
/// \param v the vertex to update
/// \return a new colour sampled uniformly from the set of unfixed colours at v
/// \sa Model::bs_getUnfixedColours
int ContractUpdate::handle_c1(Model &m, int v) { return bs_uniformSample(m.bs_getUnfixedColours(v)); }

/// compute the cutoff used to choose between c1 and c2
long double ContractUpdate::colouringGammaCutoff() const {
    std::vector<long double> weights = computeWeights(model.parameters->B, model.getNeighbourhoodColourCount(v));
    long double Z                    = std::accumulate(weights.begin(), weights.end(), 0.0L);
    return pow(model.parameters->B, weights[c1]) * unfixedCount / Z;
}

/// compute the cutoff used to set the bounding chain
long double ContractUpdate::boundingListGammaCutoff() const {
    return model.bs_getUnfixedColours(v).count() /
           (model.parameters->q - model.parameters->Delta * (1 - model.parameters->B));
}

/// update the colouring using the seed
/// \sa colouringGammaCutoff
void ContractUpdate::updateColouring() {
    try {
        model.setColour(v, gamma < colouringGammaCutoff() ? c1 : c2);
    } catch (const std::runtime_error &err) {
        throw std::runtime_error("Contract update (vertex " + std::to_string(v) +
                                 ") failed with exception: " + err.what());
    } catch (const std::invalid_argument &err) {
        throw std::runtime_error("Contract updated (vertex " + std::to_string(v) +
                                 ") failed with invalid_argument:" + err.what());
    }
}

/// update the bounding chain using the seed
/// \sa boundingListGammaCutoff
void ContractUpdate::updateBoundingChain() {
    if (gamma > boundingListGammaCutoff()) {
        model.setBoundingList(v, {c2});
    } else {
        model.setBoundingList(v, {c1, c2});
    }
}

/// constructor for initializing a compress update which chooses c1
/// \param model the model to update
/// \param v the vertex to update
/// \param bs_A the set A, common to all the compress updates in a neighbourhood
/// \sa Model::bs_generateA
CompressUpdate::CompressUpdate(Model &model, int v, const BoundingList &bs_A)
    : CompressUpdate(model, v, bs_uniformSample(bs_A.C()), bs_A) {}

/// constructor for initializing a compress update which accepts c1
/// \param model the model to update
/// \param v the vertex to update
/// \param bs_A the set A, common to all the compress updates in a neighbourhood
/// \sa Model::bs_generateA
CompressUpdate::CompressUpdate(Model &model, int v, int c1, const BoundingList &bs_A) : Update(model, v, c1), A(bs_A) {}

/// compute the cutoff used to choose between c1 and c2
/// \sa updateColouring
long double CompressUpdate::gammaCutoff() const {
    std::vector<long double> weights = computeWeights(model.parameters->B, model.getNeighbourhoodColourCount(v));
    long double Z                    = std::accumulate(weights.begin(), weights.end(), 0.0L);
    return (model.parameters->q - model.parameters->Delta) * weights[c1] / Z;
}

/// generate a sample from the set A
int CompressUpdate::sampleFromA() const {
    std::vector<long double> weights = computeWeights(model.parameters->B, model.getNeighbourhoodColourCount(v));

    //	compute the denominator used to determine when to accept a colour as a sample
    long double Z = 0;
    for (int i = 0; i < weights.size(); i++) {
        if (A[i]) {
            Z += weights[i];
        }
    }

    long double tau_x_Denominator = tau * Z;
    long double total             = 0;
    for (int c : getIndexVector<BoundingList>(A)) {
        if (total + weights[c] > tau_x_Denominator) {
            return c;
        }

        total += weights[c];
    }

    throw std::runtime_error("No sample generated from A (likely caused by rounding error).");
}

/// update the colouring using the seed
/// \sa gammaCutoff
void CompressUpdate::updateColouring() {
    try {
        model.setColour(v, gamma < gammaCutoff() ? c1 : sampleFromA());
    } catch (const std::runtime_error &err) {
        throw std::runtime_error("Compress update (vertex + " + std::to_string(v) +
                                 ") failed with exception: " + err.what());
    } catch (const std::invalid_argument &err) {
        throw std::runtime_error("Compress updated (vertex + " + std::to_string(v) +
                                 ") failed with invalid_argument:" + err.what());
    }
}

/// update the bounding list using the seed
void CompressUpdate::updateBoundingChain() {
    BoundingList bs(model.parameters->q);
    bs.set((unsigned long)c1);
    bs |= A;
    model.boundingChain[v] = bs;
}

