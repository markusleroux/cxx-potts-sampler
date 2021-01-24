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
        explicit Update(Model &model, unsigned v, unsigned c_1) : v(v), c_1(c_1), model(model) {}

        static long double unit_dist();
        static unsigned int bs_sample(const boost::dynamic_bitset<>& bs);
		static std::vector<long double> computeWeights(long double B, std::vector<unsigned> counts);

        virtual void updateColouring() = 0;
        virtual void updateBoundingChain() = 0;

		Model &model;
		const unsigned v;
		const unsigned c_1;
		const long double gamma = unit_dist();
};


#endif //POTTSSAMPLER_UPDATE_H
