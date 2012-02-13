#ifndef TREE_HPP
#define TREE_HPP

#include <list>

#include "network.hpp"

#define ROOT_NODE 0

class TreeSolution
{
public:
    Network *network;

    int *pred;
    int *depth;
    int *thread;

    long long *potential;

    Arc ***treearcs;

    std::list<Arc*> T;

    inline TreeSolution(Network *network)
    {
        this->network = network;

        this->pred = new int[network->num_nodes];
        this->depth = new int[network->num_nodes];
        this->thread = new int[network->num_nodes];

        this->potential = new long long[network->num_nodes];

        this->treearcs = new Arc**[network->num_nodes];
        for (int i = 0; i < network->num_nodes; ++i)
        {
            this->treearcs[i] = new Arc*[network->num_nodes];
            for (int j = 0; j < network->num_nodes; ++j)
            {
                this->treearcs[i][j] = NULL;
            }
        }

        this->determine_initial_tree();
        this->calc_initial_tree_structure();
    }

    void determine_initial_tree();
    void calc_initial_tree_structure();

    bool has_arc(int from, int to);
    Arc* get_arc(int from, int to);

    void update(std::list<Arc*> F, std::list<Arc*> B,
                long long theta,
                Arc *entering,
                Arc* leaving,
                int common_predecessor);

    void update_tree(Arc* entering, Arc* leaving, int join);
    void update_thread_parent(int jOut, int iOut, int jNew, int iNew, int join);
    void update_depth_pot(int jOut, int iOut, int jNew, int iNew);

    long long solution_value();

    void print_structure();
};

#endif
