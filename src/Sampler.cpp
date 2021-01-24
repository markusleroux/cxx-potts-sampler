//
// Created by Markus Le Roux on 2021-01-25.
//

#include "Sampler.h"
#include "Compress.h"
#include "Contract.h"
#include <boost/variant.hpp>

void Sampler::updateColourWithSeeds(const std::vector<boost::variant<Compress, Contract>> &seeds) {
	for (auto &seed : seeds) {

	}
}
