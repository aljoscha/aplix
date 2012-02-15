#include <iostream>

#include "parser.hpp"
#include "network.hpp"
#include "tree.hpp"
#include "simplex.hpp"

int main(int argc, char **argv) {
    std::string filename = "";
    bool print_arcs = false;
    if (argc > 2) {
        filename = std::string(argv[argc-1]);
        print_arcs = true;
    } else {
        filename = std::string(argv[1]);
    }

    Network *network = parse_nwk<Network>(filename);

    NWSimplex *simplex = new NWSimplex(network, 500, 11);

    int solution_state = simplex->compute_solution();

    if (solution_state == SOLUTION_UNBOUNDED) {
        std::cout << "unbounded" << std::endl;
    } else if (solution_state == SOLUTION_INFEASIBLE) {
        std::cout << "infeasible" << std::endl;
    } else {
        std::cout << "value: " << simplex->solution_value() << std::endl;

        if (print_arcs) {
            std::list<Arc*> *arcs = simplex->sorted_solution_arcs();
            for (std::list<Arc*>::iterator it = arcs->begin();
                    it != arcs->end(); ++it) {
                Arc *arc = (*it);
                std::cout << arc->v << " " << arc->w << " " << arc->flow << std::endl;
            }
        }
    }

    return 0;
}
