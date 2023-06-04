//
// Created by Markus Le Roux on 2021-01-25.
//

#include "Sampler.h"

/// \class updateVisitor
/// \brief visitor for use with boost::variant<Compress, Contract>
/// \sa updateColourWithSeeds
class updateVisitor : public boost::static_visitor<> {
public:
  template <typename T> void operator()(T &update) const {
    try {
      update.updateColouring();
    } catch (const std::runtime_error &err) {
      // do nothing for now
      throw err;
    }
  }
};

/// update the colouring in the model using a vector of seeds
/// \sa iteration
/// \sa history
void Sampler::updateColourWithSeeds(iteration_t &seeds) {
  for (boost::variant<Compress, Contract> seed : seeds) {
    boost::apply_visitor(updateVisitor(), seed);
  }
}

/// apply a single iteration of the algorithm
iteration_t Sampler::iteration() {
  iteration_t seeds;

  //	Phase One
  BoundingList A(model.q);
  for (int v = 0; v < model.getSize(); v++) {
    //		set A for the neighbourhood of v
    A = model.bs_generateA(v, model.Delta);
    for (int w : model.getNeighboursIndex(v)) {
      if (w > v) {
        seeds.emplace_back(Compress(model, w, A));
        boost::get<Compress>(seeds.back()).update();
      }
    }

    seeds.emplace_back(Contract(model, v));
    boost::get<Contract>(seeds.back()).update();
  }

  //	Phase Two
  int v;
  for (int i = 0; i < T; i++) {
    // choose v uniformly at random
    BoundingList bl(model.getSize());
    bl.flip();
    v = Update::bs_uniformSample(bl);
    seeds.emplace_back(Contract(model, v));
    boost::get<Contract>(seeds.back()).update();
  }

  return seeds;
}

/// sample from the anti-ferromagnetic Potts model using the algorithm
void Sampler::sample() {
  //	iterate until boundingChainIsConstant holds
  int t;
  for (t = 0; not boundingChainIsConstant(); t++) {
    try {
      writeHistory(iteration());
    } catch (const std::runtime_error &err) {
      throw std::runtime_error("Iteration " + std::to_string(t) + ": " +
                               err.what());
    }
  }

  //	apply history
  model.setBoundingListChecks(false);
  t = 0;
  for (auto it = ++history.rbegin(); it != history.rend(); it++, t++) {
    try {
      updateColourWithSeeds(*it);
    } catch (const std::runtime_error &err) {
      throw std::runtime_error("Iteration " + std::to_string(t) +
                               " (from seed): " + err.what());
    }
  }
}

/// generate the time to run the second phase in order to ensure expected
/// polynomial run-time
int Sampler::generateT(const Model &m) {
  return m.getSize() + 1 + m.getEdgeCount() +
         (int)(pow(m.getSize(), 2) *
               (m.q - m.Delta * (1 - m.B) / (m.q - m.Delta * (3 - m.B))));
}

/// predicate which holds if the bounding lists have size one at every vertex of
/// the graph
bool Sampler::boundingChainIsConstant() const {
  boundingchain_t boundingChain = model.getBoundingChain();
  return std::all_of(boundingChain.begin(), boundingChain.end(),
                     [](const BoundingList &bs) { return bs.count() == 1; });
}
