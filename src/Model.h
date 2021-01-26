//
// Created by Markus Le Roux on 2021-01-23.
//

#ifndef POTTSSAMPLER_MODEL_H
#define POTTSSAMPLER_MODEL_H

#include "Graph.h"
#include <random>

// TODO: t_boundinglist
//  - bounding list should be a class
//  - atMostKUp as method
//  - qComplement as method
//  - Graph::getIndexVector moved


/// \brief a class for holding parameters of the model, the colourings and the bounding chain
class Model : public Graph {
	private:
//      TODO: consider:
//       - Model::Delta -> Graph::Delta
//       - Model::B -> Sampler::B

		std::vector<unsigned> colouring;
		std::vector<boost::dynamic_bitset<>> boundingChain;
		unsigned int q;
		unsigned int Delta;
		long double B;
		static std::mt19937 mersene_gen;
		bool checkBoundingList = true;

		void atMostKUp(boost::dynamic_bitset<> &bs, unsigned int k) const;

		boost::dynamic_bitset<> bs_getUnfixedColours(unsigned int v) const;

		boost::dynamic_bitset<> bs_getFixedColours(unsigned int v) const;

		boost::dynamic_bitset<> UnionOfBoundingLists(const std::vector<unsigned int> &vertices) const;

		unsigned int m(unsigned int v, unsigned int c) const;

		unsigned int m_Q(unsigned int v, unsigned int c) const;

		/// setter for the checkBoundingList property
		void setBoundingListChecks(bool b) { checkBoundingList = b; }

		/// setter for bounding list
		void setBoundingList(unsigned int v, const std::vector<unsigned int> &boundingList);

		/// getter for the bounding list at vertex v
		boost::dynamic_bitset<> getBoundingList(unsigned int v) const { return boundingChain[v]; }

		/// getter for the bounding chain
		std::vector<boost::dynamic_bitset<>> getBoundingChain() const { return boundingChain; }

		boost::dynamic_bitset<> bs_generateA(unsigned int v, unsigned int size) const;

		friend class Update;

		friend class Compress;

		friend class Contract;

		friend class Sampler;

	public:
		Model(unsigned int n,
		      unsigned int q,
		      unsigned int Delta,
		      long double B,
		      const std::list<std::pair<unsigned int, unsigned int>> &edges);

/// constructor for some generic graphs
/// \param type type of graph, may be one of {"cycle", "complete"}
/// \param n the number of vertices in the graph
/// \param q the number of colours
/// \param Delta the maximum degree of the graph
/// \param B the strength of the interactions between vertices
/// \return a model object
		static Model genericModelConstructor(const std::string &type,
		                                     unsigned int n,
		                                     unsigned int q,
		                                     unsigned int Delta,
		                                     long double B) {
			std::list<std::pair<unsigned, unsigned>> edges;
			if (type == "cycle") {
				for (int i = 0; i + 1 < n; i++) { edges.emplace_back(i, i + 1); }
				if (n > 2) { edges.emplace_back(n - 1, 0); }
			} else if (type == "complete") {
				for (int i = 0; i < n; i++) {
					for (int j = 0; j < n; j++) {
						if (j != i) { edges.emplace_back(i, j); }
					}
				}
			} else {
				throw std::invalid_argument("Invalid graph type.");
			}

			return Model(n, q, Delta, B, edges);
		}

		int getColour(unsigned int v) const { return colouring[v]; }

		void setColour(unsigned int v, unsigned int c);

		std::vector<unsigned int> getColouring() { return colouring; }

		std::vector<unsigned int> getUnfixedColours(unsigned int v) const;

		std::vector<unsigned int> getFixedColours(unsigned int v) const;

		std::vector<unsigned int> getNeighbourhoodColourCount(unsigned int v) const;

		std::vector<unsigned> getBoundingListIndex(unsigned int v) const { return getIndexVector(getBoundingList(v)); }

		void sample();
};

// Overload <<
static std::ostream &operator<<(std::ostream &out, const Model &model) {
	for (unsigned v = 0; v < model.getSize(); v++) {
		out << v << " (" << model.getColour(v) << ") : {" << model.getNeighboursIndex(v) << "}\n";
	}
	return out;
}

#endif //POTTSSAMPLER_MODEL_H
