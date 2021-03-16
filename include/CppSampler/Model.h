//
// Created by Markus Le Roux on 2021-01-23.
//

#ifndef POTTSSAMPLER_MODEL_H
#define POTTSSAMPLER_MODEL_H

#include "Graph.h"
#include <random>

// TODO:
//  - move Graph::getIndexVector

class BoundingList : public boost::dynamic_bitset<> {
public:
  BoundingList(unsigned q, const std::vector<unsigned> &boundingList);
  explicit BoundingList(unsigned q);
  BoundingList() = default;

  void atMostKUp(unsigned int k);
  static BoundingList unionOfLists(const std::vector<BoundingList> &lists);
  BoundingList C() const;
};

typedef std::vector<BoundingList> boundingchain_t;
typedef std::vector<unsigned> colouring_t;

/// \brief a class for holding parameters of the model, the colourings and the
/// bounding chain
class Model : public Graph {
private:
  //      TODO: consider:
  //       - Model::Delta -> Graph::Delta
  //       - Model::B -> Sampler::B

  colouring_t colouring;
  boundingchain_t boundingChain;
  unsigned int q;
  unsigned int Delta;
  long double B;
  static std::mt19937 mersene_gen;
  bool checkBoundingList = true;

  BoundingList bs_getUnfixedColours(unsigned int v) const;

  BoundingList bs_getFixedColours(unsigned int v) const;

  BoundingList
  UnionOfBoundingLists(const std::vector<unsigned int> &vertices) const;

  unsigned int m_Q(unsigned int v, unsigned int c) const;

  /// setter for the checkBoundingList property
  void setBoundingListChecks(bool b) { checkBoundingList = b; }

  /// setter for bounding list
  void setBoundingList(unsigned int v,
                       const std::vector<unsigned int> &boundingList);

  /// getter for the bounding list at vertex v
  BoundingList getBoundingList(unsigned int v) const {
    return boundingChain[v];
  }

  /// getter for the bounding chain
  boundingchain_t getBoundingChain() const { return boundingChain; }

  BoundingList bs_generateA(unsigned int v, unsigned int size) const;

  friend class Update;

  friend class Compress;

  friend class Contract;

  friend class Sampler;

public:
  Model(unsigned int n, unsigned int q, unsigned int Delta, long double B,
        const std::list<edge_t> &edges);

  Model(unsigned int n, unsigned int q, unsigned int Delta, long double B,
        const std::string &type);

  /// get the colour of a specific vertex
  int getColour(unsigned int v) const { return colouring[v]; }

  void setColour(unsigned int v, unsigned int c);

  /// get the current colouring of the graph
  colouring_t getColouring() { return colouring; }

  std::vector<unsigned int> getUnfixedColours(unsigned int v) const;

  std::vector<unsigned int> getFixedColours(unsigned int v) const;

  std::vector<unsigned int> getNeighbourhoodColourCount(unsigned int v) const;

  std::vector<unsigned> getBoundingListIndex(unsigned int v) const {
    return getIndexVector(getBoundingList(v));
  }

  void sample();
};

// Overload <<
static std::ostream &operator<<(std::ostream &out, const Model &model) {
  for (unsigned v = 0; v < model.getSize(); v++) {
    out << v << " (" << model.getColour(v) << ") : {"
        << model.getNeighboursIndex(v) << "}\n";
  }
  return out;
}

#endif // POTTSSAMPLER_MODEL_H
