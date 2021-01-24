//
// Created by Markus Le Roux on 2021-01-24.
//

#include "Contract.h"
#include "Update.h"

#define q 10
#define B 0.5

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
	return 0;
}

long double Contract::boundingListGammaCutoff() const {
	return 0;
}

void Contract::updateColouring() {

}

void Contract::updateBoundingChain() {

}
