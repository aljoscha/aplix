#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <iostream>
#include <list>
#include <climits>

#define ARC_STATE_L -1
#define ARC_STATE_T 0
#define ARC_STATE_U 1


class Arc {
  public:
    int v,w;
    long flow, capacity, cost;
    long compare_value;
    bool artificial;
    char state;

    inline Arc(int v, int w,
               bool artificial,
               long capacity,
               long cost,
               long flow) {
        this->v = v;
        this->w = w;
        this->artificial = artificial;
        this->capacity = capacity;
        this->cost = cost;
        this->flow = flow;
        state = ARC_STATE_L;
    }

    inline int get_neighbor(int node) {
		if (v == node) {
			return w;
		}

		if (w == node) {
			return v;
		}
		// should never happen
		return -1;
	}

};

bool operator==(const Arc &lhs, const Arc &rhs);

class Node {
  public:
	int id;
	long demand;
    std::list<Arc*> outgoing;

    inline Node(int id, long demand) : id(id), demand(demand) {}

    inline void add_outgoing(Arc *arc) {
        outgoing.push_back(arc);
    }
};

class Network {
  public:
    long max_cost;
	int num_nodes;

	Node **nodes;

    inline Network(int num_nodes) : num_nodes(num_nodes) {
        this->nodes = new Node*[num_nodes];
        this->max_cost = 0;
    }

    inline void add_node(int node, long demand) {
        nodes[node] = new Node(node, demand);
    }

    inline Arc* add_artificial_arc(int from, int to) {
		Arc *arc = new Arc(from, to, true, LONG_MAX, this->max_cost, 0);
		nodes[from]->add_outgoing(arc);
		return arc;
	}

    inline Arc* add_arc(int from, int to, long cost, long capacity) {
		Arc *arc = new Arc(from, to, false, capacity, cost, 0);
        max_cost += arc->cost;
		nodes[from]->add_outgoing(arc);
		return arc;
	}
};

#endif
