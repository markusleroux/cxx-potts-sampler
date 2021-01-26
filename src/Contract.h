//
// Created by Markus Le Roux on 2021-01-24.
//

#ifndef POTTSSAMPLER_CONTRACT_H
#define POTTSSAMPLER_CONTRACT_H

#include "Update.h"

/// \brief a class for holding contract type updates; used by Sampler class
class Contract : public Update {
	private:
		unsigned int unfixedCount;
		unsigned int c2;

		Contract(Model &model, unsigned int v, unsigned int c1);

		long double colouringGammaCutoff() const;

		long double boundingListGammaCutoff() const;

		static unsigned int handle_c1(Model &model, unsigned int v);

	public:
		Contract(Model &model, unsigned int v);

		void updateColouring() override;

		void updateBoundingChain() override;

};


#endif //POTTSSAMPLER_CONTRACT_H
