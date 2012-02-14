#ifndef TREE_HPP
#define TREE_HPP

#include <list>

#include "network.hpp"

#define ROOT_NODE 0

class TreeSolution {
  private:
    Network *network;

    Arc ***treearcs;

    void determine_initial_tree();
    void calc_initial_tree_structure();

    void update_tree(Arc* entering, Arc* leaving, int join);
    void update_thread_parent(int jOut, int iOut, int jNew, int iNew, int join);
    void update_depth_pot(int jOut, int iOut, int jNew, int iNew);

    void print_structure();

  public:
    int *pred;
    int *depth;
    int *thread;
    long long *potential;

    inline TreeSolution(Network *network) : network(network) {
        pred = new int[network->num_nodes];
        depth = new int[network->num_nodes];
        thread = new int[network->num_nodes];

        potential = new long long[network->num_nodes];

        treearcs = new Arc**[network->num_nodes];
        for (int i = 0; i < network->num_nodes; ++i) {
            treearcs[i] = new Arc*[network->num_nodes];
            for (int j = 0; j < network->num_nodes; ++j) {
                treearcs[i][j] = NULL;
            }
        }

        determine_initial_tree();
        calc_initial_tree_structure();
    }

    bool has_arc(int from, int to);
    Arc* get_arc(int from, int to);

    void update(std::list<Arc*> F, std::list<Arc*> B,
                long long theta,
                Arc *entering,
                Arc* leaving,
                int common_predecessor);


    long long solution_value();
};

#endif
