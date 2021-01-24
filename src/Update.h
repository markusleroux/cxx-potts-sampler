//
// Created by Markus Le Roux on 2021-01-24.
//

#ifndef POTTSSAMPLER_UPDATE_H
#define POTTSSAMPLER_UPDATE_H

#include <random>
#include <vector>
#include "Model.h"

class Update {
	private:
		static std::mt19937 mersene_gen;

	protected:
        explicit Update(Model &model, unsigned v, unsigned c1) : v(v), c1(c1), model(model) {}

        static long double unit_dist();
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
		static unsigned int sampleFromDist(const std::vector<T>& weights);

		static unsigned int bs_uniformSample(const boost::dynamic_bitset<> &bs);
};


#endif //POTTSSAMPLER_UPDATE_H
