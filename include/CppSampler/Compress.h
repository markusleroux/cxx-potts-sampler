//
// Created by Markus Le Roux on 2021-01-23.
//

#ifndef POTTSSAMPLER_COMPRESS_H
#define POTTSSAMPLER_COMPRESS_H

#include <vector>
#include "Model.h"
#include "Update.h"

/// \brief a class for holding compress type updates; used by Sampler class
class Compress : public Update {
	private:
		const long double tau = unitDist();
		const boost::dynamic_bitset<> A;

		Compress(Model &model, unsigned int v, unsigned int c1, const boost::dynamic_bitset<> &bs_A);

		long double gammaCutoff() const;

		unsigned int sampleFromA() const;

	public:
		Compress(Model &model, unsigned int v, const boost::dynamic_bitset<> &bs_A);

		void updateColouring() override;

		void updateBoundingChain() override;
};


#endif //POTTSSAMPLER_COMPRESS_H
