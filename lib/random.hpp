#ifndef POTTSSAMPLER_RANDOM_H
#define POTTSSAMPLER_RANDOM_H

#include <boost/dynamic_bitset.hpp>
#include <random>

extern std::mt19937 mersene_gen;
extern std::uniform_real_distribution<double> dist;

/// template for sampling from the distribution described by weights
/// \tparam weight_type the type of the elements of weights
/// \param weights a vector of weights, such that the probability the function
/// returns i is proportional to w_i \return a sample from the distribution
/// described by weights
template<typename weight_type>
int sampleFromDist(const std::vector<weight_type> &weights) {
    std::discrete_distribution<int> dist(weights.begin(), weights.end());
    return dist(mersene_gen);
}

/// a method for sampling from the uniform distribution over the set bits of a
/// bitset
int bs_uniformSample(const boost::dynamic_bitset<> &bs);

/// a method for sampling from the uniform distribution over the interval [0, 1]
long double unitSample();

#endif  // POTTSSAMPLER_RANDOM_H
