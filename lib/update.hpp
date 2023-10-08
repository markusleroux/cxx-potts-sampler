#ifndef POTTSSAMPLER_UPDATE_H
#define POTTSSAMPLER_UPDATE_H

#include <vector>

#include "model.hpp"
#include "random.hpp"

std::vector<long double> computeWeights(long double B, std::vector<int> counts);

/// \brief an abstract base class for the compress and contract update types
class Update {
   protected:
    explicit Update(Model &m, int v, int c1) : v(v), c1(c1), model(m) {}

   public:
    void update() {
        updateBoundingChain();  // bounding chain must be updated before the colouring
        updateColouring();
    }

    virtual void updateColouring()     = 0;
    virtual void updateBoundingChain() = 0;

   protected:
    Model &model;
    const int v;
    const int c1;
    const long double gamma = unitSample();
};

/// \brief a class for holding contract type updates; used by Sampler class
class ContractUpdate : public Update {
   public:
    ContractUpdate(Model &model, int v);

   protected:
    ContractUpdate(Model &model, int v, int c1);

   public:
    void updateColouring() override;
    void updateBoundingChain() override;

   protected:
    long double colouringGammaCutoff() const;
    long double boundingListGammaCutoff() const;

    static int handle_c1(Model &model, int v);

    int unfixedCount;
    int c2;
};

/// \brief a class for holding compress type updates; used by Sampler class
class CompressUpdate : public Update {
   public:
    CompressUpdate(Model &model, int v, const BoundingList &bs_A);

   protected:
    CompressUpdate(Model &model, int v, int c1, const BoundingList &bs_A);

   public:
    void updateColouring() override;
    void updateBoundingChain() override;

   protected:
    long double gammaCutoff() const;

    int sampleFromA() const;

    const BoundingList A;
    const long double tau = unitSample();
};

#endif  // POTTSSAMPLER_UPDATE_H
