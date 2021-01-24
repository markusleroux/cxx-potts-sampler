//
// Created by Markus Le Roux on 2021-01-23.
//

#include "Compress.h"
#include "Model.h"
#include "Update.h"

Compress::Compress(Model &model, unsigned int v, const boost::dynamic_bitset<>& bs_A) : Update(model, v, bs_sample(~bs_A)) {};

