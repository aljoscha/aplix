#ifndef TREE_HPP
#define TREE_HPP

#include <list>
#include <vector>

#include "network.hpp"

#define ROOT_NODE 0

#define ARC_UP 0
#define ARC_DOWN 1

class TreeSolution {
  private:
    Network *network;

    void determine_initial_tree();
    void calc_initial_tree_structure();

    void update_tree(Arc* entering, Arc* leaving, int join);
    void update_thread_parent(int jOut, int iOut, int jNew, int iNew, int join);
    void update_arc_dir(int jOut, int iOut, int jNew, int iNew, Arc *entering);
    void update_depth_pot(int jOut, int iOut, int jNew, int iNew);

    void print_structure();

  public:
    int *pred;
    int *depth;
    int *thread;
    long *potential;
    Arc **basic_arcs;
    char *basic_arc_dirs;

    inline TreeSolution(Network *network) : network(network) {
        pred = new int[network->num_nodes];
        depth = new int[network->num_nodes];
        thread = new int[network->num_nodes];
        potential = new long[network->num_nodes];
        basic_arcs = new Arc*[network->num_nodes];
        basic_arc_dirs = new char[network->num_nodes];

        determine_initial_tree();
        calc_initial_tree_structure();
    }

    void update(const std::vector<Arc*> &F, const std::vector<Arc*> &B,
                long theta,
                Arc *entering,
                Arc* leaving,
                int common_predecessor);


    long solution_value();
};

#endif
