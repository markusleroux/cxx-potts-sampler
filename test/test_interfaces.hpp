#include "model.hpp"
#include "sampler.hpp"
#include "update.hpp"

struct ModelTestInterface : Model {
    template<class... Args>
    ModelTestInterface(Args&&... args) : Model(std::forward<Args>(args)...) {}

    void setColour_p(int v, int c) { return setColour(v, c); }
    int m_Q_p(int v, int c) const { return m_Q(v, c); }

    BoundingList bs_getUnfixedColours_p(int v) const { return bs_getUnfixedColours(v); }
    BoundingList bs_generateA_p(int v, int size) const { return bs_generateA(v, size); }
    void setBoundingList_p(int v, const std::vector<int> &boundingList) { setBoundingList(v, boundingList); }
};


struct UpdateTestInterface : Update {
    static std::vector<long double> computeWeights_p(long double B, std::vector<int> counts) {
        return computeWeights(B, counts);
    }
};


struct SamplerTestInterface : Sampler {
    explicit SamplerTestInterface(Model &model) : Sampler(model) {}

    void updateColourWithSeeds_p(Epoch &seeds) { updateColourWithEpoch(seeds); }

    bool boundingChainIsConstant_p() const { return boundingChainIsConstant(model.getBoundingChain()); }

    Epoch epoch() { return epoch(); }
};
