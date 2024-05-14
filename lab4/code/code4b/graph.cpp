/*********************************************
 * file:	~\code4b\graph.h                  *
 * remark:implementation of undirected graphs *
 **********************************************/

#include <iostream>
#include <algorithm>
#include <format>
#include <cassert>     // assert
#include <limits>      // std::numeric_limits
#include <algorithm>   // std::make_heap(), std::pop_heap(), std::push_heap()

#include "graph.h"
#include "dsets.h"

#include <ranges>
 // Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero

 // -- CONSTRUCTORS

 // Create a graph with n vertices and no vertices
Graph::Graph(int n) : table(n + 1), size{ n }, n_edges{ 0 } {
	assert(n >= 1);
}

Graph::Graph(const std::vector<Edge>& V, int n) : Graph{ n } {
	for (auto e : V) {
		insertEdge(e);
	}
}

// -- MEMBER FUNCTIONS

// insert undirected edge e
// update weight if edge e is present
void Graph::insertEdge(const Edge& e) {
	assert(e.from >= 1 && e.from <= size);
	assert(e.to >= 1 && e.to <= size);

	auto edge_insertion = [&T = this->table, &n = this->n_edges](const Edge& e1) {
		if (auto it = std::find_if(begin(T[e1.from]), end(T[e1.from]),
			[e1](const Edge& ed) { return e1.links_same_nodes(ed); });
			it == end(T[e1.from])) {
			T[e1.from].push_back(e1);  // insert new edge e1
			++n;                       // increment the counter of edges
		}
		else {
			it->weight = e1.weight;  // update the weight
		}
		};

	edge_insertion(e);
	edge_insertion(e.reverse());
}

// remove undirected edge e
void Graph::removeEdge(const Edge& e) {
	assert(e.from >= 1 && e.from <= size);
	assert(e.to >= 1 && e.to <= size);

	auto edgeRemoval = [&T = this->table, &n = this->n_edges](const Edge& e1) {
		auto it = std::find_if(begin(T[e1.from]), end(T[e1.from]),
			[e1](const Edge& ed) { return e1.links_same_nodes(ed); });

		assert(it != end(T[e1.from]));
		T[e1.from].erase(it);  // remove edge e1
		--n;                   // decrement the counter of edges
		};

	edgeRemoval(e);
	edgeRemoval(e.reverse());
}

// Prim's minimum spanning tree algorithm
void Graph::mstPrim() const {
	std::vector<int> dist(size + 1, std::numeric_limits<int>::max());
	std::vector<int> path(size + 1, 0);
	std::vector<bool> done(size + 1, false);

	// *** TODO ***

	dist[1] = 0; //initilise start node at distance 0
	done[1] = true;
	int v = 1; //start index
	int tot_weight = 0;
	Edge print_edge;


	while (true)
	{
		for (auto const& e : table[v]) //going through all adjacent edges
		{
			int u = e.to;
			if (!done[u] && dist[u] > e.weight)
			{
				dist[u] = e.weight;
				path[u] = v;
			}
		}

		int min = std::numeric_limits<int>::max();
		for (size_t i = 1; i <= size; i++) //finding the minimum distance
		{
			if (!done[i] && dist[i] < min)
			{
				min = dist[i];
				v = i;

			}
		}

		if (min == std::numeric_limits<int>::max()) //if no more vertices to visit
		{
			break; //does not look nice but works just fine
		}

		done[v] = true;

		tot_weight += min;
		print_edge = Edge(path[v], v, min);
		std::cout << print_edge << std::endl;

	} //while

	std::cout << std::endl << "total weight = " << tot_weight << std::endl;
}

// Kruskal's minimum spanning tree algorithm
void Graph::mstKruskal() const {
	// *** TODO ***

	//find smallet edge
	std::vector<Edge> edges;
	edges.reserve(size - 1);
	for (int i = 1; i <= size; i++)
	{
		for (auto const& e : table[i])
		{
			if (e.from < e.to)
			{
				edges.push_back(e); //n^2
			}
		}
	}
	//cool af range function

	DSets ds(size);
	std::ranges::make_heap(edges, std::greater<Edge>{});


	int tot_weight = 0;
	int counter = 0;

	while (counter < size - 1) {
		//save and remove the smallest edge from heap
		Edge min_edge = edges.front();
		std::ranges::pop_heap(edges, std::greater<Edge>{});
		edges.pop_back();

		//min_edge.from

		int min_edge_from = ds.find(min_edge.from);
		int min_edge_to = ds.find(min_edge.to);

		if (min_edge_from != min_edge_to){
			
			ds.join(min_edge_from, min_edge_to);
			std::cout << min_edge << std::endl;
			tot_weight += min_edge.weight;
			counter++;
		}
	}

	std::cout << std::endl << "total weight = " << tot_weight << std::endl;
}

// print graph
void Graph::printGraph() const {
	std::cout << std::format("{:-<66}\n", '-');
	std::cout << "Vertex  adjacency lists\n";
	std::cout << std::format("{:-<66}\n", '-');

	for (int v = 1; v <= size; v++) {
		std::cout << std::format("{:4} : ", v);
		for (auto const& e : table[v]) {
			std::cout << std::format("({:2}, {:2}) ", e.to, e.weight);
		}
		std::cout << "\n";
	}
	std::cout << std::format("{:-<66}\n", '-');
}
