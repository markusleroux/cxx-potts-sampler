#ifndef POTTSSAMPLER_MODEL_H
#define POTTSSAMPLER_MODEL_H

#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include "state.hpp"


/// \brief a class for holding parameters of the model, the colourings and the
/// bounding chain
class Model {
   public:
    Model(std::shared_ptr<const Parameters> parameters, Graph graph);
    Model(std::shared_ptr<const Parameters> parameters, const std::vector<Graph::edge_t> &edges)
        : Model{std::move(parameters), Graph(parameters->n, edges)} {}
    Model(std::shared_ptr<const Parameters> parameters, const std::string &type)
        : Model{std::move(parameters), Graph(parameters->n, type)} {}

    int getColour(int v) const { return colouring[v]; }
    const colouring_t &getColouring() const { return colouring; }
    std::vector<int> getUnfixedColours(int v) const;
    std::vector<int> getFixedColours(int v) const;
    std::vector<int> getNeighbourhoodColourCount(int v) const;

    const boundingchain_t &getBoundingChain() const { return boundingChain; }
    std::vector<BoundingList> getBoundingLists(const std::vector<int> &vertices) const;
    BoundingList getBoundingList(int v) const { return boundingChain[v]; }
    std::vector<int> getBoundingListIndex(int v) const { return getIndexVector(getBoundingList(v)); }
    int m_Q(int v, int c) const;

    void sample();

    friend std::ostream &operator<<(std::ostream &out, const Model &model) { return out << model.graph; }

    const Graph graph;
    const std::shared_ptr<const Parameters> parameters;

   protected:
    colouring_t colouring;
    boundingchain_t boundingChain;

    bool checkBoundingList = true;

    BoundingList bs_getUnfixedColours(int v) const;
    BoundingList bs_getFixedColours(int v) const;

    void setColour(int v, int c);

    /// setter for the checkBoundingList property
    void setBoundingListChecks(bool b) { checkBoundingList = b; }

    /// setter for bounding list
    void setBoundingList(int v, const std::vector<int> &boundingList);
    BoundingList bs_generateA(int v, int size) const;

    friend class CompressUpdate;
    friend class ContractUpdate;
    friend class Sampler;
};

template<typename element_type>
std::ostream &operator<<(std::ostream &out, const std::vector<element_type> &vector) {
    if (vector.empty()) {
        return out;
    }

    for (auto it = std::begin(vector); it != std::prev(std::end(vector)); it++) {
        out << *it << ",";
    }
    out << vector.back();
    return out;
}

#endif  // POTTSSAMPLER_MODEL_H
