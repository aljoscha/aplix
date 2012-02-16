// A Parser for .nwk files as used in the network-simplex exercises in ADM
// at TU-Berlin.
//
// Author: Aljoscha Krettek

#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <climits>
#include <cctype>

using namespace std;

// Reads a network instance from the given file and stores it in a new network
// of type NetworkType. The network type must have a constructor that takes one
// int value, the number or nodes, and it must have the two method:
//
// add_node(int node_num, int demand)
// add_arc(int from, int to, long cost, long capacity)
//
// These are called whenever the respective element is parse from the file.
//
// Use it like this:
//     Network* network = parse_nwk<Network>(filename);
//
template <class NetworkType>
NetworkType* parse_nwk(std::string filename) {
    NetworkType *network = NULL;
    ifstream file(filename.c_str());

    int num_arcs = 0;
    if (file.is_open()) {
        int num_nodes;
        file >> num_nodes;
        //cout << "NUM_NODES: " << num_nodes << endl;
        network = new NetworkType(num_nodes);

        // read away the newline
        while (file.peek() != '\n') file.get();
        file.get();

        while (file.good()) {
            string line_string;
            getline(file, line_string);
            if (line_string.size() == 0) continue;
            istringstream line(line_string);

            // read the node number
            int node_num;
            int demand = 0;
            line >> node_num;

            // read away whitespace
            while (line.peek() == ' ' || line.peek() == '\t') line.get();

            if (line.peek() == '(') {
                // skip to the demand
                while (line.peek() != ')') line.get();
                line.get();
            }

            if (line.peek() == ',') {
                line.get();
                // read away whitespace
                while (line.peek() == ' ' || line.peek() == '\t') line.get();
                line >> demand;
            }

            //cout << "node: " << node_num << " " << demand << endl;
            network->add_node(node_num, demand);


            while (line.peek() != ':') line.get();
            line.get();

            //cout << "READING ARCS\n";

            // now the arcs ...

            while (line.good()) {
                // read away whitespace
                while (line.peek() == ' ' || line.peek() == '\t') line.get();
                if (!line.good()) break;

                int target;
                line >> target;

                // read ", "
                line.get();
                line.get();

                int cost;
                line >> cost;

                int capacity = LONG_MAX;
                if (line.peek() != ';') {
                    // read ", "
                    line.get();
                    line.get();
                    line >> capacity;
                }
                line.get();

                //cout << "ARC: "
                //    << target << " " << cost << " " << capacity << endl;
                network->add_arc(node_num, target, cost, capacity);
                ++num_arcs;
            }


        }
    } else {
        cout << "Unable to open file " << filename;
    }
    return network;
}

#endif
