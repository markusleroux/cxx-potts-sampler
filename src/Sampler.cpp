//
// Created by Markus Le Roux on 2021-01-25.
//

#include "Sampler.h"
#include "Compress.h"
#include "Contract.h"
#include <boost/variant.hpp>

class updateVisitor : public boost::static_visitor<> {
	public:
		template<typename T>
		void operator()(T &update) const {
			update.updateColouring();
		}
};

void Sampler::updateColourWithSeeds(std::vector<boost::variant<Compress, Contract>> &seeds) {
	for (boost::variant<Compress, Contract> seed : seeds) {
		boost::apply_visitor(updateVisitor(), seed);
	}
}

std::vector<boost::variant<Compress, Contract>> Sampler::iteration() {
	std::vector<boost::variant<Compress, Contract>> seeds;

//	Phase One
	boost::dynamic_bitset<> A;
	for (unsigned int v = 0; v < model.getSize(); v++) {
		A = model.bs_generateA(v, model.Delta);
		for (unsigned int w : model.getNeighboursIndex(v)) {
			if (w > v) {
				seeds.emplace_back(Compress(model, w, A));
				boost::get<Compress>(seeds.back()).update();
			}
		}

		seeds.emplace_back(Contract(model, v));
		boost::get<Contract>(seeds.back()).update();
	}

//	Phase Two
	unsigned int v;
	for (int i = 0; i < T; i++) {
		v = Update::bs_uniformSample(~boost::dynamic_bitset<>(model.getSize()));
		seeds.emplace_back(Contract(model, v));
		boost::get<Contract>(seeds.back()).update();
	}

	return seeds;
}

void Sampler::sample() {
	for (unsigned int t = 0; not boundingChainIsConstant(); t++) { writeHistory(iteration()); }

	std::cout << "multi it";
	model.setBoundingListChecks(false);
	for (auto it = ++history.rbegin(); it != history.rend(); it++) {
		updateColourWithSeeds(*it);
	}
}

unsigned int Sampler::generateT(const Model &m) {
	unsigned int n = m.getSize();
	return n + 1 + m.getEdgeCount() +
	       (unsigned int) (pow(n, 2) * (m.q - m.Delta * (1 - m.B) / (m.q - m.Delta * (3 - m.B))));
}

bool Sampler::boundingChainIsConstant() const {
	std::vector<boost::dynamic_bitset<>> boundingChain = model.getBoundingChain();
	return std::all_of(boundingChain.begin(), boundingChain.end(),
	                   [](const boost::dynamic_bitset<> &bs) { return bs.count() == 1; });
}
