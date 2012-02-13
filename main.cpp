#include <iostream>

#include "parser.hpp"
#include "network.hpp"
#include "tree.hpp"
#include "simplex.hpp"

int main()
{
    Network *network = parse_nwk("example-networks/hu.nwk");

    NWSimplex *simplex = new NWSimplex(network, 500, 11);

    int solution_state = simplex->compute_solution();

    if (solution_state == SOLUTION_UNBOUNDED)
    {
        std::cout << "unbounded" << std::endl;
    }
    else if (solution_state == SOLUTION_INFEASIBLE)
    {
        std::cout << "infeasible" << std::endl;
    }
    else
    {
        std::cout << "value: " << simplex->solution_value() << std::endl;
    }
    return 0;
}
