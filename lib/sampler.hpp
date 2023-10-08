#ifndef POTTSSAMPLER_SAMPLER_H
#define POTTSSAMPLER_SAMPLER_H

#include <variant>

#include "model.hpp"
#include "update.hpp"

//	TODO: optimize to avoid unnecessary copies

struct Epoch {
    std::vector<CompressUpdate> phaseOneHistory{};
    std::vector<ContractUpdate> phaseTwoHistory{};
};

/// generate the time to run the second phase in order to ensure expected
/// polynomial run-time
static int getPhaseTwoIters(const Graph graph, const std::shared_ptr<const Parameters> parameters) {
    return graph.size() + 1 + graph.numEdges() +
           pow(graph.size(), 2) *
                (parameters->q - parameters->Delta * (1 - parameters->B) / (parameters->q - parameters->Delta * (3 - parameters->B)));
}

/// \brief a class which holds the sampling algorithm
class Sampler {
   public:
    explicit Sampler(Model &model) : model(model) {};
    Sampler(Model &model, int phaseTwoIters) : model(model), phaseTwoIters(phaseTwoIters) {};

    void sample();

   protected:
    Epoch epoch();
    void updateColourWithEpoch(Epoch&);
    void recordEpoch(const Epoch &epoch) { history.emplace_back(epoch); }

    Model &model;
    int phaseTwoIters = getPhaseTwoIters(model.graph, model.parameters);
    std::vector<Epoch> history;
};

#endif  // POTTSSAMPLER_SAMPLER_H
