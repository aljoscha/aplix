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
    void update_thread_parent(Node* jOut, Node* iOut, Node* jNew, Node* iNew, int join);
    void update_arc_dir(Node* jOut, Node* iOut, Node* jNew, Node* iNew, Arc *entering);
    void update_depth_pot(Node* jOut, Node* iOut, Node* jNew, Node* iNew);

    void print_structure();

  public:
    Node **nodes;

    inline TreeSolution(Network *network) : network(network) {
        nodes = network->nodes;
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
