//
// Created by Markus Le Roux on 2021-01-25.
//

#include "Sampler.h"
#include "Compress.h"
#include "Contract.h"
#include <boost/variant.hpp>

/// \class updateVisitor
/// \brief visitor for use with booost::variant<Compress, Contract>
/// \sa updateColourWithSeeds
class updateVisitor : public boost::static_visitor<> {
	public:
		template<typename T>
		void operator()(T &update) const {
			update.updateColouring();
		}
};

/// update the colouring in the model using a vector of seeds
/// \sa iteration
/// \sa history
void Sampler::updateColourWithSeeds(std::vector<boost::variant<Compress, Contract>> &seeds) {
	for (boost::variant<Compress, Contract> seed : seeds) {
		boost::apply_visitor(updateVisitor(), seed);
	}
}

/// apply a single iteration of the algorithm
std::vector<boost::variant<Compress, Contract> > Sampler::iteration() {
	std::vector<boost::variant<Compress, Contract> > seeds;

//	Phase One
	boost::dynamic_bitset<> A;
	for (unsigned int v = 0; v < model.getSize(); v++) {
//		set A for the neighbourhood of v
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
	for (unsigned int i = 0; i < T; i++) {
		// choose v uniformly at random
		v = Update::bs_uniformSample(~boost::dynamic_bitset<>(model.getSize()));
		seeds.emplace_back(Contract(model, v));
		boost::get<Contract>(seeds.back()).update();
	}

	return seeds;
}

/// sample from the anti-ferromagnetic Potts model using the algorithm
void Sampler::sample() {
//	iterate until boundingChainIsConstant holds
	for (unsigned int t = 0; not boundingChainIsConstant(); t++) { writeHistory(iteration()); }

//	apply history
	model.setBoundingListChecks(false);
	for (auto it = ++history.rbegin(); it != history.rend(); it++) {
		updateColourWithSeeds(*it);
	}
}

/// generate the time to run the second phase in order to ensure expected polynomial run-time
unsigned int Sampler::generateT(const Model &m) {
	return m.getSize() + 1 + m.getEdgeCount() +
	       (unsigned int) (pow(m.getSize(), 2) * (m.q - m.Delta * (1 - m.B) / (m.q - m.Delta * (3 - m.B))));
}

/// predicate which holds if the bounding lists have size one at every vertex of the graph
bool Sampler::boundingChainIsConstant() const {
	std::vector<boost::dynamic_bitset<> > boundingChain = model.getBoundingChain();
	return std::all_of(boundingChain.begin(), boundingChain.end(),
	                   [](const boost::dynamic_bitset<> &bs) { return bs.count() == 1; });
}
