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

//		TODO: optimize to use pointers or avoid unnecessary copies

/// \brief a class which holds the sampling algorithm
class Sampler {
	private:
		Model &model;
		unsigned int T;
		std::vector<std::vector<boost::variant<Compress, Contract>>> history;

		std::vector<boost::variant<Compress, Contract>> iteration();

		void updateColourWithSeeds(std::vector<boost::variant<Compress, Contract>> &seeds);

///		write the lists of seeds to the history in place
		void writeHistory(const std::vector<boost::variant<Compress, Contract>> &seeds) { history.emplace_back(seeds); }

		bool boundingChainIsConstant() const;

		static unsigned int generateT(const Model &m);

		unsigned int generateT() { return generateT(model); }

	public:
		/// constructor for the sampler class which chooses the number of iterations of phase two automatically
		explicit Sampler(Model &model) : Sampler(model, generateT(model)) {};

		/// constructor for the sampler class which accepts the number of iterations of phase two as a parameter
		Sampler(Model &model, unsigned int T) : model(model), T(T) {};

		void sample();
};


#endif //POTTSSAMPLER_SAMPLER_H
