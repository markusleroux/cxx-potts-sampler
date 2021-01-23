//
// Created by Markus Le Roux on 2021-01-23.
//

#ifndef POTTSSAMPLER_MODEL_H
#define POTTSSAMPLER_MODEL_H


#include <vector>
#include <list>
#include <boost/dynamic_bitset.hpp>
#include "Graph.h"

class Model : public Graph {
    private:
        std::vector<unsigned> colouring;
        std::vector<boost::dynamic_bitset<>> boundingChain;

        static boost::dynamic_bitset<> atMostKUp(boost::dynamic_bitset<> bs, unsigned int k);
        boost::dynamic_bitset<> bs_getUnfixedColours(unsigned int v);
        boost::dynamic_bitset<> bs_getFixedColours(unsigned int v);
        boost::dynamic_bitset<> UnionOfBoundingLists(const std::vector<unsigned int> &vertices);

    public:
        Model(unsigned int n, const std::list<std::pair<unsigned int, unsigned int>>& edges);
        int getColour(unsigned int v) { return colouring[v]; }
        void setColour(unsigned int v, unsigned int c);

        std::vector<unsigned int> getColouring() { return colouring; }
        std::vector<unsigned int> getUnfixedColours(unsigned int v);
        std::vector<unsigned int> getFixedColours(unsigned int v);
        std::vector<unsigned int> getNeighbourhoodColourCount(unsigned int v);
        unsigned int m(unsigned int v, unsigned int c);

        boost::dynamic_bitset<> getBoundingList(unsigned int v) { return boundingChain[v]; }
        void setBoundingList(unsigned int v, const std::vector<unsigned int> &boundingList);
        std::vector<boost::dynamic_bitset<>> getBoundingChain() { return boundingChain; }
        unsigned int m_Q(unsigned int v, unsigned int c);
        
        boost::dynamic_bitset<> bs_generateA(unsigned int v, unsigned int size);
};

#endif //POTTSSAMPLER_MODEL_H
