//
// Created by Markus Le Roux on 2021-01-24.
//

#ifndef POTTSSAMPLER_UPDATE_H
#define POTTSSAMPLER_UPDATE_H

#include <random>
#include <vector>
#include "Model.h"

/// \brief an abstract base class for the compress and contract update types
class Update {
	protected:
		Model &model;
		const unsigned v;
		const unsigned c1;
		const long double gamma = unitDist();

		explicit Update(Model &m, unsigned v, unsigned c1) : v(v), c1(c1), model(m) {}

		static std::vector<long double> computeWeights(long double B, std::vector<unsigned> counts);

		virtual void updateColouring() = 0;

		virtual void updateBoundingChain() = 0;

		static long double unitDist();

		static unsigned int bs_uniformSample(const boost::dynamic_bitset<> &bs);

		/// template for sampling from the distribution described by weights
		/// \tparam weight_type the type of the elements of weights
		/// \param weights a vector of weights, such that the probability the function returns i is proportional to w_i
		/// \return a sample from the distribution described by weights
		template<typename weight_type>
		static unsigned int sampleFromDist(const std::vector<weight_type> &weights) {
			std::discrete_distribution<unsigned int> dist(weights.begin(), weights.end());
			return dist(Model::mersene_gen);
		}

		friend class Sampler;

	public:
		/// update the bounding chain, then update the colouring
		void update() {
//			note that the bounding chain must be updated before the colouring
			updateBoundingChain();
			updateColouring();
		}
};


#endif //POTTSSAMPLER_UPDATE_H
