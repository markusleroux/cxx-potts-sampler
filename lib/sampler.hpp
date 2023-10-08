#ifndef POTTSSAMPLER_SAMPLER_H
#define POTTSSAMPLER_SAMPLER_H

#include <boost/variant.hpp>

#include "model.hpp"
#include "update.hpp"

//	TODO: optimize to avoid unnecessary copies

typedef std::vector<boost::variant<CompressUpdate, ContractUpdate>> iteration_t;

/// generate the time to run the second phase in order to ensure expected
/// polynomial run-time
static int getPhaseTwoIters(const Graph graph, const std::shared_ptr<const Parameters> parameters) {
    return graph.getSize() + 1 + graph.getEdgeCount() +
           static_cast<int>(pow(graph.getSize(), 2) *
                            (parameters->q - parameters->Delta * (1 - parameters->B) /
                                                   (parameters->q - parameters->Delta * (3 - parameters->B))));
}

/// \brief a class which holds the sampling algorithm
class Sampler {
   public:
    /// constructor for the sampler class which chooses the number of iterations
    /// of phase two automatically
    explicit Sampler(Model &model) : Sampler(model, getPhaseTwoIters(model.graph, model.parameters)){};

    /// constructor for the sampler class which accepts the number of iterations
    /// of phase two as a parameter
    Sampler(Model &model, int phaseTwoIters) : model(model), phaseTwoIters(phaseTwoIters){};

    void sample();

   protected:
    iteration_t iteration();
    void updateColourWithSeeds(iteration_t &seeds);

    ///  write the lists of seeds to the history (in place)
    void writeHistory(const iteration_t &seeds) { history.emplace_back(seeds); }

    bool boundingChainIsConstant() const;

    Model &model;
    int phaseTwoIters;
    std::vector<iteration_t> history{};

};

#endif  // POTTSSAMPLER_SAMPLER_H
