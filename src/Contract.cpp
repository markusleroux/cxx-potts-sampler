//
// Created by Markus Le Roux on 2021-01-24.
//

#include "Contract.h"
#include "Update.h"

#define q 10
#define B 0.5
#define Delta 5

Contract::Contract(Model &model, unsigned int v) : Update(model, v, handle_c1(v)) {
	unfixedCount = model.bs_getUnfixedColours(v).count();

	std::vector<long double> weights(q, 0);

//	TODO: see similarity with Update::computeWeights
	for (unsigned c : model.getFixedColours(v)) {
		weights[c] = (long double) pow(B, model.m_Q(v, c));
	}

	std::vector<unsigned int> w(q, 0);

	c2 = sampleFromDist<long double>(weights);
}

unsigned int Contract::handle_c1(unsigned int v) {
	try {
		return bs_uniformSample(model.bs_getUnfixedColours(v));
	} catch (const std::invalid_argument & e) {
		return 0;
	}
}

long double Contract::colouringGammaCutoff() const {
	std::vector<long double> weights = computeWeights(B, model.getNeighbourhoodColourCount(v));
	long double Z = std::accumulate(weights.begin(), weights.end(), (long double) 0.0);
	return pow(B, weights[c1])*unfixedCount / Z;
}

long double Contract::boundingListGammaCutoff() const {
	return model.bs_getUnfixedColours(v).count() / (q - Delta * (1 - B));
}

void Contract::updateColouring() {
	if (gamma >= colouringGammaCutoff()) {
		model.setColour(v, c1);
	} else { model.setColour(v, c2); }
}

void Contract::updateBoundingChain() {
	if (gamma > boundingListGammaCutoff()) {
		model.setBoundingList(v, {c1});
	} else { model.setBoundingList(v, {c1, c2}); }
}
