//
// Created by Markus Le Roux on 2021-01-24.
//

#include "Contract.h"
#include <random>
#include "Model.h"
#include "Update.h"

long double Update::unitDist() {
    static std::uniform_real_distribution<double> dist(0.0, 1.0);

    return dist(Model::mersene_gen);
}

unsigned int Update::bs_uniformSample(const boost::dynamic_bitset<>& bs) {
	std::vector<unsigned int> weights(bs.size(), 0);
	for (int i = 0; i < bs.size(); i++) {
		if (bs[i]) { weights[i] = 1; }
	}

	return sampleFromDist<unsigned int>(weights);
}

std::vector<long double> Update::computeWeights(long double B, std::vector<unsigned int> counts) {
	std::vector<long double> weights(counts.size());
	std::transform(counts.begin(), counts.end(), weights.begin(), [B](unsigned int m_c){ return pow(B, m_c); });
	return weights;
}