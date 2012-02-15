#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include "parser.hpp"
#include "network.hpp"
#include "tree.hpp"
#include "simplex.hpp"

namespace po = boost::program_options;

int main(int argc, char **argv) {

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Produce help message.")
        ("verbose,v", "Print runtimes of different parts of the program.")
        ("print-arcs,a", "Print the arcs of the solution with their flow.")
        ("input-file", po::value<std::string>(), "Input file.")
    ;

    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(p).run(), vm);
    po::notify(vm);
    

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    boost::posix_time::ptime mst1 = boost::posix_time::microsec_clock::local_time();
    std::string filename = vm["input-file"].as<std::string>();
    Network *network = parse_nwk(filename);
    boost::posix_time::ptime mst2 = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration msdiff = mst2 - mst1;
    if (vm.count("verbose")) {
        std::cout << "Parse time: " << msdiff.total_milliseconds() << std::endl;
    }

    mst1 = boost::posix_time::microsec_clock::local_time();
    NWSimplex *simplex = new NWSimplex(network, 500, 11);
    mst2 = boost::posix_time::microsec_clock::local_time();
    msdiff = mst2 - mst1;
    if (vm.count("verbose")) {
        std::cout << "Constructor time: " << msdiff.total_milliseconds() << std::endl;
    }

    mst1 = boost::posix_time::microsec_clock::local_time();
    int solution_state = simplex->compute_solution();
    mst2 = boost::posix_time::microsec_clock::local_time();
    msdiff = mst2 - mst1;
    if (vm.count("verbose")) {
        std::cout << "Simplex time: " << msdiff.total_milliseconds() << std::endl;
        std::cout << "Iterations: " << simplex->num_iterations << std::endl;
    }


    if (solution_state == SOLUTION_UNBOUNDED) {
        std::cout << "unbounded" << std::endl;
    } else if (solution_state == SOLUTION_INFEASIBLE) {
        std::cout << "infeasible" << std::endl;
    } else {
        std::cout << "value: " << simplex->solution_value() << std::endl;

        if (vm.count("print-arcs")) {
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
