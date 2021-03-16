//
// Created by Markus Le Roux on 2021-01-23.
//

#include "Compress.h"
#include "Model.h"
#include "Update.h"

/// constructor for initializing a compress update which chooses c1
/// \param model the model to update
/// \param v the vertex to update
/// \param bs_A the set A, common to all the compress updates in a neighbourhood
/// \sa Model::bs_generateA
Compress::Compress(Model &model, unsigned int v, const BoundingList &bs_A)
    : Compress(model, v, bs_uniformSample(bs_A.C()), bs_A){};

/// constructor for initializing a compress update which accepts c1
/// \param model the model to update
/// \param v the vertex to update
/// \param bs_A the set A, common to all the compress updates in a neighbourhood
/// \sa Model::bs_generateA
Compress::Compress(Model &model, unsigned int v, unsigned int c1,
                   const BoundingList &bs_A)
    : Update(model, v, c1), A(bs_A){};

/// compute the cutoff used to choose between c1 and c2
/// \sa updateColouring
long double Compress::gammaCutoff() const {
  std::vector<long double> weights =
      computeWeights(model.B, model.getNeighbourhoodColourCount(v));
  long double Z =
      std::accumulate(weights.begin(), weights.end(), (long double)0.0);
  return (model.q - model.Delta) * weights[c1] / Z;
}

/// generate a sample from the set A
unsigned int Compress::sampleFromA() const {
  std::vector<long double> weights =
      computeWeights(model.B, model.getNeighbourhoodColourCount(v));

  //	compute the denominator used to determine when to accept a colour as a
  // sample
  long double Z = 0;
  for (int i = 0; i < weights.size(); i++) {
    if (A[i]) {
      Z += weights[i];
    }
  }

  long double tau_x_Denominator = tau * Z;
  long double total = 0;
  for (unsigned int c : Model::getIndexVector<BoundingList>(A)) {
    if (total + weights[c] > tau_x_Denominator) {
      return c;
    } else {
      total += weights[c];
    }
  }

  throw std::runtime_error(
      "No sample generated from A (likely caused by rounding error).");
}

/// update the colouring using the seed
/// \sa gammaCutoff
void Compress::updateColouring() {
  try {
    if (gamma < gammaCutoff()) {
      model.setColour(v, c1);
    } else {
      model.setColour(v, sampleFromA());
    }
  } catch (const std::runtime_error &err) {
    throw std::runtime_error("Compress update (vertex + " + std::to_string(v) +
                             ") failed with exception: " + err.what());
  } catch (const std::invalid_argument &err) {
    throw std::runtime_error("Compress updated (vertex + " + std::to_string(v) +
                             ") failed with invalid_argument:" + err.what());
  }
}

/// update the bounding list using the seed
void Compress::updateBoundingChain() {
  BoundingList bs(model.q);
  bs.set(c1);
  bs |= A;

  model.boundingChain[v] = bs;
}
