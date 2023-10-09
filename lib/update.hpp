#ifndef POTTSSAMPLER_UPDATE_H
#define POTTSSAMPLER_UPDATE_H

#include <vector>

#include "random.hpp"
#include "sampler.hpp"
#include "state.hpp"

std::vector<long double> pow(long double temperature, std::vector<int> counts);

class Update {
   protected:
    Update(const State &m, int v, int c1) : v(v), c1(c1), state(m) {}

   public:
    const State &state;
    const int v;
    const int c1;
    const long double gamma = unitSample();
};

class ContractUpdate : public Update {
   public:
    ContractUpdate(const State &state, int v) : ContractUpdate(state, v, proposeC1(state, v)) {}

   protected:
    ContractUpdate(const State &, int v, int c1);

   public:
    int getNewColour() const { return gamma < colouringGammaCutoff() ? c1 : c2; }

    BoundingList getNewBoundingChain() const {
        return {
            state.parameters.maxColours,
            gamma > boundingListGammaCutoff() ? std::vector<int>{c2}
                : std::vector<int>{c1, c2}
        };
    }

   protected:
    long double colouringGammaCutoff() const;
    long double boundingListGammaCutoff() const;
    static int proposeC1(const State &model, int v);

    int unfixedCount;

   public:
    const int c2;
};

class CompressUpdate : public Update {
   public:
    CompressUpdate(const State &state, int v, const BoundingList &bs_A)
        : CompressUpdate(state, v, uniformSample(bs_A.flip_copy()), bs_A) {}

   protected:
    CompressUpdate(const State &state, int v, int c1, const BoundingList &bs_A) : Update(state, v, c1), A(bs_A) {}

   public:
    int getNewColour() const { return gamma < gammaCutoff() ? c1 : sampleFromA(); }

    BoundingList getNewBoundingChain() const {
        BoundingList bs = A;
        bs.set(c1);
        return bs;
    }

   protected:
    long double gammaCutoff() const;
    int sampleFromA() const;

    const BoundingList A;
    const long double tau = unitSample();
};

#endif  // POTTSSAMPLER_UPDATE_H
