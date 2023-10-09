#ifndef POTTSSAMPLER_RANDOM_H
#define POTTSSAMPLER_RANDOM_H

#include <boost/dynamic_bitset.hpp>
#include <random>

extern std::mt19937 mersene_gen;

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

/// select random set bit
int uniformSample(const boost::dynamic_bitset<> &bs);

/// sample from the uniform distribution over the interval [0, 1]
long double unitSample();

#endif  // POTTSSAMPLER_RANDOM_H
