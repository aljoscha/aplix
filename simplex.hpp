#ifndef SIMPLEX_HPP
#define SIMPLEX_HPP

#include "network.hpp"
#include "tree.hpp"

#include <vector>
#include <iostream>

#define SOLUTION_INFEASIBLE 0
#define SOLUTION_UNBOUNDED 1
#define SOLUTION_OPTIMAL 2
#define SOLUTION_UNSOLVED 3

class Cycle {
  public:
    long theta;
	Arc* blocking;
	int common_predecessor;
    std::vector<Arc*> F;
    std::vector<Arc*> B;
};

class NWSimplex {
  private:
    Network *network;
    TreeSolution *tree;

    int solution_state;

    int current_startnode;
    unsigned int max_list_size;
    unsigned int max_min_its;

    std::vector<Arc*> candidate_list;
    Cycle cycle;

    bool perform_major_iteration();
    void fill_candidate_list();
    void compute_cycle(Arc* entering);
    void recalc_redcosts();
    Arc* get_best_arc();

  public:
    int num_iterations;

    inline NWSimplex(Network *network, int max_list_size, int max_min_its) {
        this->network = network;
        this->max_list_size = max_list_size;
        this->max_min_its = max_min_its;

        tree = new TreeSolution(network);

        num_iterations = 0;
        solution_state = SOLUTION_UNSOLVED;
        current_startnode = 0;
    }

    int compute_solution();
    long solution_value();
    std::list<Arc*>* sorted_solution_arcs();
};

#endif
