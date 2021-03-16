//
// Created by Markus Le Roux on 2021-01-24.
//

#include "Update.h"
#include "Contract.h"
#include "Model.h"
#include <random>

/// a method for sampling from the uniform distribution on the interval [0, 1]
long double Update::unitDist() {
  static std::uniform_real_distribution<double> dist(0.0, 1.0);
  return dist(Model::mersene_gen);
}

/// a method for sampling from the uniform distribution over the set bits of a
/// bitset
unsigned int Update::bs_uniformSample(const BoundingList &bs) {
  std::vector<unsigned int> weights(bs.size(), 0);
  for (int i = 0; i < bs.size(); i++) {
    if (bs[i]) {
      weights[i] = 1;
    }
  }

  return sampleFromDist<unsigned int>(weights);
}

/// a method which applies the map c -> B^c to a vector
/// \param B the base B
/// \param counts a vector containing the exponent
/// \return a vector of weights B^c
std::vector<long double>
Update::computeWeights(long double B, std::vector<unsigned int> counts) {
  std::vector<long double> weights(counts.size());
  std::transform(counts.begin(), counts.end(), weights.begin(),
                 [B](unsigned int m_c) { return pow(B, m_c); });
  return weights;
}
