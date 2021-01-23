#include <vector>
#include <list>
#include <boost/dynamic_bitset.hpp>

class Graph {
    private:
        std::vector<std::vector<bool>> adjacencyMatrix;
        std::vector<int> colouring;
        std::vector<boost::dynamic_bitset<>> boundingChain;

        // Should probably move this
        template <typename bitset_t> std::vector<int> getIndexVector(bitset_t bs);
        boost::dynamic_bitset<> atMostKUp(boost::dynamic_bitset<> bs, int k);

        boost::dynamic_bitset<> bs_getUnfixedColours(int v);
        boost::dynamic_bitset<> bs_getFixedColours(int v);
        boost::dynamic_bitset<> UnionOfBoundingLists(int v);

    public:
        Graph(unsigned n, std::list<std::pair<unsigned, unsigned>> edges);

        int getColour(int v) { return colouring[v]; }
        void setColour(int v, int c);

        std::vector<int> getUnfixedColours(int v);
        std::vector<int> getFixedColours(int v);

        std::vector<int> getColouring() { return colouring; }
        std::vector<int> getNeighbourhoodColourCount(int v);

        int m_Q(int v, int c);
        int m(int v, int c);

        std::vector<int> getNeighboursIndex(int v) { return getIndexVector<std::vector<bool>>(adjacencyMatrix[v]); }
        std::vector<std::vector<bool>> getAdjacencyMatrix() { return adjacencyMatrix; }

        boost::dynamic_bitset<> getBoundingList(int v) { return boundingChain[v]; }
        void setBoundingList(int v, std::vector<int> boundingList);
        std::vector<boost::dynamic_bitset<>> getBoundingChain() { return boundingChain; }
        
        boost::dynamic_bitset<> bs_generateA(int v, int size, std::vector<int> vertices);
};

