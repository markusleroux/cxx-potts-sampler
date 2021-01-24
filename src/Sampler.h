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

#define q 10
#define B 0.5
#define Delta 5

class Sampler {
	private:
		Model model;
		unsigned int T;

//		TODO: optimize to use pointers or avoid unnecessary copies
		std::vector<std::vector<boost::variant<Compress, Contract>>> history;

		std::vector<boost::variant<Compress, Contract>> iteration();
		void updateColourWithSeeds(const std::vector<boost::variant<Compress, Contract>> &seeds);

		void writeHistory(const std::vector<boost::variant<Compress, Contract>> &seeds) {
			history.push_back(seeds);
		}

		bool boundingChainIsConstant() const {
			std::vector<boost::dynamic_bitset<>> boundingChain = model.getBoundingChain();
			return std::all_of(boundingChain.begin(), boundingChain.end(), [](const boost::dynamic_bitset<> &bs){ return bs.count() == 1; });
		}

//		static function for use in constructor
		static unsigned int generateT(const Model &m) {
			unsigned int n = m.getSize();
			return n + 1 + m.getEdgeCount() + (unsigned int) (pow(n, 2) * (q - Delta * (1 - B) / (q - Delta * (3 - B))));
		}
		unsigned int generateT() { return generateT(model); }

	public:
		explicit Sampler(Model &model) : Sampler(model, generateT(model)) {};
		Sampler(Model &model, unsigned int T) : model(model), T(T) {};


		void sample();
		static bool checkParameters() {
			return (q > 2 * Delta) && (Delta >= 3) && (B > 1 - (long double)(q - 2 * Delta) / Delta) && (B < 1);
		}
};


#endif //POTTSSAMPLER_SAMPLER_H
