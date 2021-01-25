//
// Created by Markus Le Roux on 2021-01-23.
//
#include "Graph.h"

Graph::Graph(unsigned n, const std::list<std::pair<unsigned, unsigned>>& edges) {
    // Initialize matrix of size n x n
    adjacencyMatrix = std::vector<std::vector<bool>>(n, std::vector<bool>(n));

    // Populate adjacencyMatrix with edges
    for (std::pair<unsigned, unsigned> edge : edges) {
        adjacencyMatrix[edge.first][edge.second] = true;
        adjacencyMatrix[edge.second][edge.first] = true;
    }
}

unsigned Graph::getEdgeCount() const {
	unsigned int total = 0;
	for (std::vector<bool> neighbours : adjacencyMatrix ) {
		total += std::count(neighbours.begin(), neighbours.end(), true);
	}
	return total / 2;
}
