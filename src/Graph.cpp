//
// Created by Markus Le Roux on 2021-01-23.
//
#include "Graph.h"

/// constructor for the graph class
/// \param n the number of vertices in the graph
/// \param edges
Graph::Graph(unsigned n, const std::list<edge_t> &edges) {
  // Initialize matrix of size n x n
  adjacencyMatrix = adjmatrix_t(n, std::vector<bool>(n));

  // Populate adjacencyMatrix with edges
  for (edge_t edge : edges) {
    adjacencyMatrix[edge.first][edge.second] = true;
    adjacencyMatrix[edge.second][edge.first] = true;
  }
}

Graph::Graph(unsigned n, const std::string &type)
    : Graph(n, buildEdgeSet(n, type)) {}

/// helper function for constructing a set of edges
/// \param n the number of vertices in the graph
/// \param type the type of the graph (one of cycle, complete)
std::list<edge_t> Graph::buildEdgeSet(unsigned n, const std::string &type) {
	std::list<edge_t> edges;
	if (type == "cycle") {
		for (int i = 0; i + 1 < n; i++) {
			edges.emplace_back(i, i + 1);
		}
		if (n > 2) {
			edges.emplace_back(n - 1, 0);
		}
	} else if (type == "complete") {
		int j;
		for (int i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				if (i != j) {
					edges.emplace_back(i, j);
				}
			}
		}
	} else {
		throw std::invalid_argument("Invalid graph type.");
	}
	return edges;
}

/// return the number of edges in the graph
unsigned Graph::getEdgeCount() const {
  unsigned total{0};
  for (std::vector<bool> neighbours : adjacencyMatrix) {
    total += std::count(neighbours.begin(), neighbours.end(), true);
  }
  return total / 2;
}
