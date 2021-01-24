//
// Created by Markus Le Roux on 2021-01-24.
//

#ifndef POTTSSAMPLER_CONTRACT_H
#define POTTSSAMPLER_CONTRACT_H

#include "Update.h"

class Contract : public Update {
	private:
		const boost::dynamic_bitset<> A;
		unsigned int unfixedCount;
		unsigned int c2;

	public:
		Contract(Model &model, unsigned int v);

		long double colouringGammaCutoff() const;
		long double boundingListGammaCutoff() const;

		void updateColouring() override;
		void updateBoundingChain() override;

		unsigned int handle_c1(unsigned int v);

};


#endif //POTTSSAMPLER_CONTRACT_H
