#include "random.hpp"

/// a seed for random number generators
std::mt19937 mersene_gen{std::random_device{}()};

std::uniform_real_distribution<double> dist(0.0, 1.0);

int bs_uniformSample(const boost::dynamic_bitset<> &bs) {
    std::vector<int> weights(bs.size(), 0);
    for (int i = 0; i < bs.size(); i++) {
        weights[i] = bs[i];
    }
    return sampleFromDist<int>(weights);
}

long double unitSample() { return dist(mersene_gen); }
