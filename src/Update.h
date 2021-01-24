//
// Created by Markus Le Roux on 2021-01-24.
//

#ifndef POTTSSAMPLER_UPDATE_H
#define POTTSSAMPLER_UPDATE_H

#include <random>
#include <vector>
#include "Model.h"

class Update {
	protected:
        explicit Update(Model &model, unsigned v, unsigned c1) : v(v), c1(c1), model(model) {}
		static std::vector<long double> computeWeights(long double B, std::vector<unsigned> counts);

        virtual void updateColouring() = 0;
        virtual void updateBoundingChain() = 0;
		void update() {
			updateBoundingChain();
			updateColouring();
		}

		Model &model;
		const unsigned v;
		const unsigned c1;
		const long double gamma = unit_dist();

	public:
		template <typename T>
		static unsigned int sampleFromDist(const std::vector<T>& weights) {
			std::discrete_distribution<unsigned int> dist(weights.begin(), weights.end());
			return dist(Model::mersene_gen);
		}

		static long double unit_dist();
		static unsigned int bs_uniformSample(const boost::dynamic_bitset<> &bs);
};


#endif //POTTSSAMPLER_UPDATE_H
