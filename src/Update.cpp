//
// Created by Markus Le Roux on 2021-01-24.
//

#include <random>
#include "Model.h"
#include "Update.h"

long double Update::unit_dist() {
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);

    return dist(mt);
}

unsigned int Update::bs_sample(const boost::dynamic_bitset<>& bs) {
    if (bs.empty() || bs.none()) {
        throw std::invalid_argument("Cannot sample from bitset without set bits.");
    }

    std::random_device rd;
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<int> distribution(1,bs.count());

    unsigned i = bs.find_first();
    for (int j = distribution(mt); j > 1; j--) { i = bs.find_next(i); }

    return i;
}

std::vector<long double> Update::computeWeights(long double B, std::vector<unsigned int> counts) {
	std::vector<long double> weights(counts.size());
	std::transform(counts.begin(), counts.end(), weights.begin(), [B](unsigned int m_c){ return pow(B, m_c); });
}
