#include "sampler.hpp"
#include "random.hpp"


/// update the colouring in the model using a vector of seeds
/// \sa iteration
/// \sa history
void Sampler::updateColourWithEpoch(Epoch &epoch) {
    for (auto& iteration : epoch.phaseOneHistory) {
        iteration.updateColouring();
    }
    for (auto& iteration : epoch.phaseTwoHistory) {
        iteration.updateColouring();
    }
}

/// apply a single iteration of the algorithm
Epoch Sampler::epoch() {
    Epoch epoch;

    // Phase One
    BoundingList A(model.parameters->q);
    for (int v = 0; v < model.graph.size(); v++) {
        // set A for the neighbourhood of v
        A = model.bs_generateA(v, model.parameters->Delta);
        for (int w : model.graph.getNeighboursIndex(v)) {
            if (w > v) {
                epoch.phaseOneHistory.emplace_back(model, w, A);
                epoch.phaseOneHistory.back().update();
            }
        }

        epoch.phaseTwoHistory.emplace_back(model, v);
        epoch.phaseTwoHistory.back().update();
    }

    // Phase Two
    int v;
    for (int i = 0; i < phaseTwoIters; i++) {
        // choose v uniformly at random
        BoundingList bl(model.graph.size());
        bl.flip();
        v = bs_uniformSample(bl);
        epoch.phaseTwoHistory.emplace_back(model, v);
        epoch.phaseTwoHistory.back().update();
    }

    return epoch;
}

/// sample from the anti-ferromagnetic Potts model using the algorithm
void Sampler::sample() {
    // iterate until boundingChainIsConstant holds
    int t;
    for (t = 0; !boundingChainIsConstant(model.getBoundingChain()); t++) {
        try {
            recordEpoch(epoch());
        } catch (const std::runtime_error &err) {
            throw std::runtime_error("Iteration " + std::to_string(t) + ": " + err.what());
        }
    }

    // apply history
    model.setBoundingListChecks(false);
    t = 0;
    for (auto it = ++history.rbegin(); it != history.rend(); it++, t++) {
        try {
            updateColourWithEpoch(*it);
        } catch (const std::runtime_error &err) {
            throw std::runtime_error("Iteration " + std::to_string(t) + " (from seed): " + err.what());
        }
    }
}

