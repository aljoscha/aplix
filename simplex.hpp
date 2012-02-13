#ifndef SIMPLEX_HPP
#define SIMPLEX_HPP

#include "network.hpp"
#include "tree.hpp"

#define SOLUTION_INFEASIBLE 0
#define SOLUTION_UNBOUNDED 1
#define SOLUTION_OPTIMAL 2
#define SOLUTION_UNSOLVED 3

class Cycle
{
public:
    long long theta;
	Arc* blocking;
	int common_predecessor;
    std::list<Arc*> F;
    std::list<Arc*> B;

    inline Cycle(long long theta, Arc* blocking, int common_predecessor,
            std::list<Arc*> F,
            std::list<Arc*> B)
    {
        this->theta = theta;
        this->blocking = blocking;
        this->common_predecessor = common_predecessor;
        this->F = F;
        this->B = B;
    }
};

class NWSimplex
{
private:
    Network *network;
    TreeSolution *tree;

    int num_iterations;
    int solution_state;

    int current_startnode;
    unsigned int max_list_size;
    unsigned int max_min_its;

    std::list<Arc*> candidate_list;

public:
    inline NWSimplex(Network *network, int max_list_size, int max_min_its)
    {
        this->network = network;
        this->max_list_size = max_list_size;
        this->max_min_its = max_min_its;

        tree = new TreeSolution(network);

        num_iterations = 0;
        solution_state = SOLUTION_UNSOLVED;
        current_startnode = 0;
    }

    int compute_solution();
    bool perform_major_iteration();
    void fill_candidate_list();
    Cycle compute_cycle(Arc* entering);
    void recalc_redcosts();
    Arc* get_best_arc();
    long long solution_value();

    std::list<Arc*>* sorted_solution_arcs();

};

#endif
