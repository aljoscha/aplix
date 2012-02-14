#include <iostream>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "parser.hpp"
#include "network.hpp"
#include "tree.hpp"
#include "simplex.hpp"

int main(int arc, char **argv) {
    std::string filename(argv[1]);
    Network *network = parse_nwk(filename);

    NWSimplex *simplex = new NWSimplex(network, 500, 11);

    boost::posix_time::ptime mst1 = boost::posix_time::microsec_clock::local_time();

    int solution_state = simplex->compute_solution();

    boost::posix_time::ptime mst2 = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration msdiff = mst2 - mst1;
    std::cout << "Runtime: " << msdiff.total_milliseconds() << std::endl;

    if (solution_state == SOLUTION_UNBOUNDED) {
        std::cout << "unbounded" << std::endl;
    } else if (solution_state == SOLUTION_INFEASIBLE) {
        std::cout << "infeasible" << std::endl;
    } else {
        std::cout << "value: " << simplex->solution_value() << std::endl;

        /*
        std::list<Arc*> *arcs = simplex->sorted_solution_arcs();
        
        for (std::list<Arc*>::iterator it = arcs->begin();
                it != arcs->end(); ++it)
        {
            Arc *arc = (*it);
            std::cout << arc->v << " " << arc->w << " " << arc->flow << std::endl;
        }
        */
    }

    return 0;
}
