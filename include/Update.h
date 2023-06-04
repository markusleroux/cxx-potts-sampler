//
// Created by Markus Le Roux on 2021-01-24.
//

#ifndef POTTSSAMPLER_UPDATE_H
#define POTTSSAMPLER_UPDATE_H

#include "Model.h"
#include <random>
#include <vector>

/// \brief an abstract base class for the compress and contract update types
class Update {
public:
    void update() {
        updateBoundingChain();  // bounding chain must be updated before the colouring
        updateColouring();
    }

protected:
    explicit Update(Model &m, int v, int c1) : v(v), c1(c1), model(m) {}

    static std::vector<long double> computeWeights(long double B, std::vector<int> counts);

    virtual void updateColouring() = 0;
    virtual void updateBoundingChain() = 0;

    static long double unitDist();
    static int bs_uniformSample(const BoundingList &bs);

    /// template for sampling from the distribution described by weights
    /// \tparam weight_type the type of the elements of weights
    /// \param weights a vector of weights, such that the probability the function
    /// returns i is proportional to w_i \return a sample from the distribution
    /// described by weights
    template <typename weight_type>
    static int sampleFromDist(const std::vector<weight_type> &weights) {
        std::discrete_distribution<int> dist(weights.begin(), weights.end());
        return dist(Model::mersene_gen);
    }

    Model &model;
    const int v;
    const int c1;
    const long double gamma = unitDist();

    friend class Sampler;
    friend class UpdateTest;
};

/// \brief a class for holding contract type updates; used by Sampler class
class Contract : public Update {
public:
  Contract(Model &model, int v);

  void updateColouring() override;
  void updateBoundingChain() override;

private:
    Contract(Model &model, int v, int c1);

    long double colouringGammaCutoff() const;
    long double boundingListGammaCutoff() const;

    static int handle_c1(Model &model, int v);

    int unfixedCount;
    int c2;

    friend class ContractTest;
};

/// \brief a class for holding compress type updates; used by Sampler class
class Compress : public Update {
public:
  Compress(Model &model, int v, const BoundingList &bs_A);

  void updateColouring() override;
  void updateBoundingChain() override;

private:
  Compress(Model &model, int v, int c1, const BoundingList &bs_A);

  long double gammaCutoff() const;

  int sampleFromA() const;

  const long double tau = unitDist();
  const BoundingList A;

  friend class CompressTest;
};

#endif // POTTSSAMPLER_UPDATE_H
