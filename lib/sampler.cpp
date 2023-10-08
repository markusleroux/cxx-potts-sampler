#include "sampler.hpp"

#include "random.hpp"


/// update the colouring in the model using a vector of seeds
/// \sa iteration
/// \sa history
void Sampler::updateColourWithSeeds(iteration_t &seeds) {
    for (boost::variant<CompressUpdate, ContractUpdate> &seed : seeds) {
        boost::apply_visitor([](auto &update) { update.updateColouring(); }, seed);
    }
}

/// apply a single iteration of the algorithm
iteration_t Sampler::iteration() {
    iteration_t seeds;

    // Phase One
    BoundingList A(model.parameters->q);
    for (int v = 0; v < model.graph.size(); v++) {
        // set A for the neighbourhood of v
        A = model.bs_generateA(v, model.parameters->Delta);
        for (int w : model.graph.getNeighboursIndex(v)) {
            if (w > v) {
                seeds.emplace_back(CompressUpdate(model, w, A));
                boost::get<CompressUpdate>(seeds.back()).update();
            }
        }

        seeds.emplace_back(ContractUpdate(model, v));
        boost::get<ContractUpdate>(seeds.back()).update();
    }

    // Phase Two
    int v;
    for (int i = 0; i < phaseTwoIters; i++) {
        // choose v uniformly at random
        BoundingList bl(model.graph.size());
        bl.flip();
        v = bs_uniformSample(bl);
        seeds.emplace_back(ContractUpdate(model, v));
        boost::get<ContractUpdate>(seeds.back()).update();
    }

    return seeds;
}

/// sample from the anti-ferromagnetic Potts model using the algorithm
void Sampler::sample() {
    // iterate until boundingChainIsConstant holds
    int t;
    for (t = 0; !boundingChainIsConstant(); t++) {
        try {
            writeHistory(iteration());
        } catch (const std::runtime_error &err) {
            throw std::runtime_error("Iteration " + std::to_string(t) + ": " + err.what());
        }
    }

    // apply history
    model.setBoundingListChecks(false);
    t = 0;
    for (auto it = ++history.rbegin(); it != history.rend(); it++, t++) {
        try {
            updateColourWithSeeds(*it);
        } catch (const std::runtime_error &err) {
            throw std::runtime_error("Iteration " + std::to_string(t) + " (from seed): " + err.what());
        }
    }
}

/// predicate which holds if the bounding lists have size one at every vertex of
/// the graph
bool Sampler::boundingChainIsConstant() const {
    boundingchain_t boundingChain = model.getBoundingChain();
    return std::all_of(boundingChain.begin(), boundingChain.end(),
                       [](const BoundingList &bs) { return bs.count() == 1; });
}
