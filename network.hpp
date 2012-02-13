#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <list>
#include <climits>

class Arc
{
public:
    int v,w;
    long long flow, capacity, cost;
    long compare_value;
    bool artificial;

    inline Arc(int v, int w, bool artificial, long capacity, long cost, long flow)
    {
        this->v = v;
        this->w = w;
        this->artificial = artificial;
        this->capacity = capacity;
        this->cost = cost;
        this->flow = flow;
    }

    inline int get_neighbor(int node)
    {
		if (v == node)
        {
			return w;
		}
		if (w == node)
        {
			return v;
		}
		// should never happen
		return -1;
	}

};

bool operator==(const Arc &lhs, const Arc &rhs)
{
    if (lhs.v != rhs.v) {
        return false;
    }

    if (lhs.w != rhs.w) {
        return false;
    }

    if (lhs.cost != rhs.cost) {
        return false;
    }

    if (lhs.capacity != rhs.capacity) {
        return false;
    }

    if (lhs.artificial != rhs.artificial) {
        return false;
    }

    return true;
}

class Node {
public:
	int id;
	long long demand;
    std::list<Arc*> outgoing;

    inline Node(int id, long long demand) : id(id), demand(demand)
    {
    }

    inline void add_outgoing(Arc *arc)
    {
        outgoing.push_back(arc);
    }
};
                    
class Network
{
public: 
    long max_cost;
	int num_nodes;

	Node **nodes;
    std::list<Arc*> arcs;
    std::list<Arc*> artificial_arcs;

    inline Network(int num_nodes)
    {
        this->num_nodes = num_nodes;
        this->nodes = new Node*[num_nodes];
    }

    inline void calc_max_cost()
    {
        this->max_cost = 0;
        std::cout << "ARCS.size(): " << arcs.size() << std::endl;
        for (std::list<Arc*>::iterator it = arcs.begin(); it != arcs.end(); ++it)
        {
            this->max_cost += (*it)->cost;
        }

        std::cout << "Max Cost: " << this->max_cost << std::endl;
    }

    inline void add_node(int node, long long demand)
    {
        //std::cout << "Add node" << std::endl;
        nodes[node] = new Node(node, demand);
    }

    inline Arc* add_artificial_arc(int from, int to)
    {
		Arc *arc = new Arc(from, to, true, LONG_MAX, this->max_cost, 0);
		artificial_arcs.push_back(arc);
		nodes[from]->add_outgoing(arc);
		return arc;
	}

    inline Arc* add_arc(int from, int to, long cost, long capacity)
    {
		Arc *arc = new Arc(from, to, false, capacity, cost, 0);
		arcs.push_back(arc);
		nodes[from]->add_outgoing(arc);
		return arc;
	}
};

#endif
