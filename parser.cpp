#include <iostream>
#include <fstream>
#include <string>
#include <climits>
#include <boost/xpressive/xpressive.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "parser.hpp"
#include "network.hpp"



static std::string NODE_PATTERN = "\\d+\\s*,?\\s*\\(\\s*\\-?\\s*\\d+\\s*,\\s*\\-?\\s*\\d+\\s*\\)(\\s*,?\\s*\\-?\\s*\\d+)?";

using namespace std;
using namespace boost::xpressive;

Network* parse_nwk(std::string filename) {
    Network *network = NULL;
    //sregex rex = sregex::compile(NODE_PATTERN.c_str());
    string line;
    ifstream file(filename.c_str());

    if (file.is_open()) {
        getline(file, line);
        int num_nodes = boost::lexical_cast<int>(line);
        //std::cout << "Num nodes: " << num_nodes << std::endl;

        // create network
        network = new Network(num_nodes);

        int currline = 2;
        while (file.good()) {
            getline (file,line);
            if (line.size() > 0) {
                std::vector<std::string> line_splits;
                boost::split(line_splits, line, boost::is_any_of(":"));

                smatch match;

				// parse node data
                std::string node_data = line_splits[0];

                /*
                if( !regex_search( node_data, match, rex ) )
                {
                    std::cout << "Illegal node description in line: "
                        << currline << std::endl;
                    return NULL;
                }
                */

                std::vector<std::string> node_parts;
                boost::split(node_parts, node_data, boost::is_any_of(","));

                int node_num = -1;
                int demand = 0;

                std::vector<std::string> num_parts;
                boost::split(num_parts, node_parts[0], boost::is_any_of(" "));

                string num_string = num_parts[0];
                node_num = boost::lexical_cast<int>(num_string);

                if (node_parts.size() > 2) {
                    string demand_string = node_parts[2];
                    boost::algorithm::trim(demand_string);
                    demand = boost::lexical_cast<int>(demand_string);
                }
                
                //std::cout << "Node: " << node_num << " " << demand << std::endl;
                network->add_node(node_num, demand);


				// parse incident arcs
				if (line_splits.size() > 1) {
					string arcs_data = line_splits[1];

                    std::vector<std::string> arcs;
                    boost::split(arcs, arcs_data, boost::is_any_of(";"));

					for (unsigned int i = 0; i < arcs.size(); ++i) {
                        boost::algorithm::trim(arcs[i]);

                        std::vector<std::string> arc_data;
                        boost::split(arc_data, arcs[i], boost::is_any_of(","));

                        if (arc_data.size() == 0) {
                            continue;
                        }

						if (arc_data.size() < 2) {
                            //std::cout << "Illegal arc desc: " << arcs[i] << std::endl;
                            continue;
                        }

                        string target_string = arc_data[0];
                        boost::algorithm::trim(target_string);
                        int target_node = boost::lexical_cast<int>(target_string);

                        if (target_node >= num_nodes) {
                            std::cout << "Illegal target node: " << target_node << std::endl;
                        }

                        boost::algorithm::trim(arc_data[1]);
                        long cost = boost::lexical_cast<int>(arc_data[1]);

                        long capacity = LONG_MAX;

                        if (arc_data.size() > 2) {
                            boost::algorithm::trim(arc_data[2]);
                            capacity = boost::lexical_cast<int>(arc_data[2]);
                        }

                        // add new arc
                        //std::cout << "Arc: " << target_node << " " << cost <<
                        //    " " << capacity << std::endl;
                        network->add_arc(node_num, target_node, cost, capacity);
					}
				}
            }
            ++currline;
        }
        file.close();
        network->calc_max_cost();
    } else {
        cout << "Unable to open file " << filename;
    }
    return network;
}
