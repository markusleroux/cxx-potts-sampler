//
// Created by Markus Le Roux on 2021-03-18.
//

#include "Model.h"
#include "Sampler.h"
#include "Update.h"

class ModelTest : public Model {
public:
  ModelTest(int n, int q, int Delta, long double B, const std::string &type)
      : Model(n, q, Delta, B, type) {}

  void setColour_p(int v, int c) { return setColour(v, c); }

  BoundingList bs_getUnfixedColours_p(int v) const {
    return bs_getUnfixedColours(v);
  }

  int m_Q_p(int v, int c) const { return m_Q(v, c); }

  BoundingList bs_generateA_p(int v, int size) const {
    return bs_generateA(v, size);
  }

  void setBoundingList_p(int v, const std::vector<int> &boundingList) {
    setBoundingList(v, boundingList);
  }
};

class UpdateTest : public Update {
public:
  static int bs_uniformSample_p(const BoundingList &bs) {
    return bs_uniformSample(bs);
  }

  static std::vector<long double> computeWeights_p(long double B,
                                                   std::vector<int> counts) {
    return computeWeights(B, counts);
  }
};

class CompressTest : public Compress {
public:
  CompressTest(Model &model, int v, const BoundingList &bs_A)
      : Compress(model, v, bs_A) {}
  int getC1() { return c1; }
};

class ContractTest : public Contract {
public:
  ContractTest(Model &model, int v) : Contract(model, v) {}
  int getC1() { return c1; }
  int getC2() { return c2; }
};

class SamplerTest : public Sampler {
public:
  explicit SamplerTest(Model &model) : Sampler(model) {}

  void updateColourWithSeeds_p(iteration_t &seeds) {
    updateColourWithSeeds(seeds);
  }

  bool boundingChainIsConstant_p() const { return boundingChainIsConstant(); }

  iteration_t iteration_p() { return iteration(); }
};