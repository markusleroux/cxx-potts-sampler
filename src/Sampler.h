//
// Created by Markus Le Roux on 2021-01-25.
//

#ifndef POTTSSAMPLER_SAMPLER_H
#define POTTSSAMPLER_SAMPLER_H


#include <boost/variant.hpp>
#include "Model.h"
#include "Update.h"
#include "Compress.h"
#include "Contract.h"

class Sampler {
	private:
		Model &model;
		unsigned int T;

//		TODO: optimize to use pointers or avoid unnecessary copies
		std::vector<std::vector<boost::variant<Compress, Contract>>> history;

		std::vector<boost::variant<Compress, Contract>> iteration();
		void updateColourWithSeeds(std::vector<boost::variant<Compress, Contract>> &seeds);

		void writeHistory(const std::vector<boost::variant<Compress, Contract>> &seeds) {
			history.emplace_back(seeds);
		}

		bool boundingChainIsConstant() const {
			std::vector<boost::dynamic_bitset<>> boundingChain = model.getBoundingChain();
			return std::all_of(boundingChain.begin(), boundingChain.end(), [](const boost::dynamic_bitset<> &bs){ return bs.count() == 1; });
		}

//		static function for use in constructor
		static unsigned int generateT(const Model &m) {
			unsigned int n = m.getSize();
			return n + 1 + m.getEdgeCount() + (unsigned int) (pow(n, 2) * (m.q - m.Delta * (1 - m.B) / (m.q - m.Delta * (3 - m.B))));
		}
		unsigned int generateT() { return generateT(model); }

	public:
		explicit Sampler(Model &model) : Sampler(model, generateT(model)) {};
		Sampler(Model &model, unsigned int T) : model(model), T(T) {};

		void sample();
};


#endif //POTTSSAMPLER_SAMPLER_H
