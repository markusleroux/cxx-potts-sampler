//
// Created by Markus Le Roux on 2021-01-24.
//

#include "Contract.h"
#include "Update.h"

/// constructor for Contract update which chooses c1
/// \param model the model to update
/// \param v the vertex to update
Contract::Contract(Model &model, unsigned int v)
    : Contract(model, v, handle_c1(model, v)) {}

/// constructor for Contract update which accepts c1
/// \param m the model to update
/// \param v the vertex to update
/// \param c1 the proposal for the new colour of v
Contract::Contract(Model &m, unsigned int v, unsigned int c1)
    : Update(m, v, c1) {
  unfixedCount = model.bs_getUnfixedColours(v).count();

  std::vector<long double> weights(model.q);

  //	TODO: see similarity with Update::computeWeights
  for (unsigned c : model.getFixedColours(v)) {
    weights[c] = (long double)pow(model.B, model.m_Q(v, c));
  }

  c2 = sampleFromDist<long double>(weights);
}

/// choose propose a new colour for the vertex v
/// \param m the model being updated
/// \param v the vertex to update
/// \return a new colour sampled uniformly from the set of unfixed colours at v
/// \sa Model::bs_getUnfixedColours
unsigned int Contract::handle_c1(Model &m, unsigned int v) {
  return bs_uniformSample(m.bs_getUnfixedColours(v));
}

/// compute the cutoff used to choose between c1 and c2
long double Contract::colouringGammaCutoff() const {
  std::vector<long double> weights =
      computeWeights(model.B, model.getNeighbourhoodColourCount(v));
  long double Z =
      std::accumulate(weights.begin(), weights.end(), (long double)0.0);
  return ((long double)pow(model.B, weights[c1])) * (long double)unfixedCount /
         Z;
}

/// compute the cutoff used to set the bounding chain
long double Contract::boundingListGammaCutoff() const {
  return model.bs_getUnfixedColours(v).count() /
         (model.q - model.Delta * (1 - model.B));
}

/// update the colouring using the seed
/// \sa colouringGammaCutoff
void Contract::updateColouring() {
  try {
    if (gamma < colouringGammaCutoff()) {
      model.setColour(v, c1);
    } else {
      model.setColour(v, c2);
    }
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
void Contract::updateBoundingChain() {
  if (gamma > boundingListGammaCutoff()) {
    model.setBoundingList(v, {c2});
  } else {
    model.setBoundingList(v, {c1, c2});
  }
}
