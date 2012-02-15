#include "tree.hpp"

#include <stack>
#include <iostream>

void TreeSolution::determine_initial_tree() {
    long demand_sum = 0;
    for (int i = 0; i < network->num_nodes; ++i) {
        demand_sum += nodes[i]->demand;
    }

    if (demand_sum != 0) {
        throw "Network is infeasible because demands do not sum up to zero.";
    }

    for (int i = 1; i < network->num_nodes; ++i) {
        Node *node = nodes[i];

        Arc *arc = NULL;
        if (node->demand < 0) {
            // this means that node has DEMAND!! eg we want to make an arc
            // INTO this node
            arc = network->add_artificial_arc(ROOT_NODE, node->id);
            arc->flow = -node->demand;
            node->basic_arc = arc;
            node->basic_arc_dir = ARC_DOWN;
        } else {
            // network.getDemand(node) >= 0
            // eg node has supply or flow conservation! want to make an arc
            // FROM this node
            arc = network->add_artificial_arc(node->id, ROOT_NODE);
            arc->flow = node->demand;
            node->basic_arc = arc;
            node->basic_arc_dir = ARC_UP;
        }
        arc->state = ARC_STATE_T;
        nodes[ROOT_NODE]->basic_arc = NULL;
    }
}

void TreeSolution::calc_initial_tree_structure() {
    nodes[ROOT_NODE]->depth = 0;
    nodes[ROOT_NODE]->pred = NULL;
    nodes[ROOT_NODE]->thread = nodes[1];
    nodes[ROOT_NODE]->potential = 0;		


    int num_nodes = network->num_nodes;

    for (int i = 1; i < num_nodes; ++i) {
        nodes[i]->depth = 1;
        nodes[i]->pred = nodes[ROOT_NODE];
        nodes[i]->thread = nodes[(i + 1) % num_nodes];
        if (nodes[i]->basic_arc_dir == ARC_DOWN) {
            nodes[i]->potential = network->max_cost;
        } else {
            nodes[i]->potential = -network->max_cost;
        }
    }
}

void TreeSolution::update(const std::vector<Arc*> &F,
                const std::vector<Arc*> &B,
                long theta,
                Arc *entering,
                Arc* leaving,
                int common_predecessor) {

    if (theta != 0) {
        for (std::vector<Arc*>::const_iterator it = F.begin();
                it != F.end(); ++it) {
            (*it)->flow += theta;
        }

        for (std::vector<Arc*>::const_iterator it = B.begin();
                it != B.end(); ++it) {
            (*it)->flow -= theta;
        }
    }

    if (leaving->flow == 0) {
        // must now be in L
        if (!leaving->artificial) {
            leaving->state = ARC_STATE_L;
        } else {
            nodes[leaving->v]->outgoing.remove(leaving);
        }
    } else {
        // must be at max capacity, so add to U
        leaving->state = ARC_STATE_U;
    }

    // entering arc must not be removed from L/U because getEnteringArcFrom*
    // already do that
    entering->state = ARC_STATE_T;

    this->update_tree(entering, leaving, common_predecessor);
}

void TreeSolution::update_tree(Arc* entering, Arc* leaving, int join)
{
    if (entering == leaving) {
        return;
    }

    int jOut, iOut, jNew = -1, iNew = -1;
    // first determine iOut and jOut
    if (nodes[leaving->v]->pred == nodes[leaving->w]) {
        jOut = leaving->w;
        iOut = leaving->v;
    } else if (nodes[leaving->w]->pred == nodes[leaving->v]) {
        jOut = leaving->v;
        iOut = leaving->w;
    } else {
        throw "Serious problem.";
    }
    // check whether V or W is in subtree of iOut
    int i = iOut;

    // Check whether blocking arc is on way from entering.V to join or from
    // entering.W to join. This tells us which side of the entering arc is
    // the root of the subtree now hanging at the entering arc.
    i = entering->w;
    while (nodes[i]->depth > nodes[join]->depth) {
        if (i == leaving->v || i == leaving->w) {
            iNew = entering->w;
            jNew = entering->v;
            break;
        }
        i = nodes[i]->pred->id;
    }
    i = entering->v;
    while (nodes[i]->depth > nodes[join]->depth) {
        if (i == leaving->v || i == leaving->w) {
            iNew = entering->v;
            jNew = entering->w;
            break;
        }
        i = nodes[i]->pred->id;
    }

    if (jNew == -1) {
        throw "Serious problem.";
    }

    Node* node_jOut = nodes[jOut];
    Node* node_iOut = nodes[iOut];
    Node* node_jNew = nodes[jNew];
    Node* node_iNew = nodes[iNew];

    update_thread_parent(node_jOut, node_iOut, node_jNew, node_iNew, join);
    update_arc_dir(node_jOut, node_iOut, node_jNew, node_iNew, entering);
    update_depth_pot(node_jOut, node_iOut, node_jNew, node_iNew);
}

void TreeSolution::update_thread_parent(Node* jOut, Node* iOut, Node* jNew,
        Node* iNew, int join) {
    bool parent_first = false;
    Node* i = NULL;
    Node* first = NULL;
    Node* last;
    Node* right;

    Node *stem, *newStem, *predStem;

    if (join == jOut->id) {
        // determine whether jNew or iNew appear first on the thread
        if (nodes[join]->pred == NULL) {
            // have to do this because ROOT_NODE might be the "join"
            for (int index = 0; index < network->num_nodes; ++index) {
                if (nodes[index]->thread == nodes[ROOT_NODE]) {
                    i = nodes[index];
                }
            }

        } else {
            i = nodes[join]->pred;
        }

        while (i->thread != iNew && i->thread != jNew) {
            i = i->thread;
        }

        if (i->thread == jNew) {
            parent_first = true;
        }

        while (i->thread != iOut) {
            i = i->thread;
        }
        first = i;
    }

    // now traverse the stem
    i = iNew;

    // calculate the last right successor for the first iteration
    while (i->thread->depth > iNew->depth) {
        i = i->thread;
    }
    right = i->thread;
    // i is the last right successor if i (iNew) now

    if (jNew->thread == iOut) {
        last = i;
        while (last->depth > iOut->depth) {
            last = last->thread;
        }
        if (last == iOut) {
            last = last->thread;
        }
    } else {
        last = jNew->thread;
    }

    jNew->thread = iNew;
    stem = iNew;
    predStem = jNew;

    while (stem != iOut) {
        // i is the last right successor at this point
        i->thread = stem->pred;

        i = stem->pred;
        while (i->thread != stem) {
            i = i->thread;
        }
        // i is the last left successor of parent
        i->thread = right;

        newStem = stem->pred;
        stem->pred = predStem;
        predStem = stem;
        stem = newStem;

        // set i to the last right successor again
        i = stem;
        while (i->thread->depth > stem->depth) {
            i = i->thread;
        }
        right = i->thread;
    }

    i->thread = last;

    if (nodes[join] == jOut) {
        if (!parent_first) {
            i = jOut;
            while (i->thread != iOut) {
                i = i->thread;
            }
            i->thread = right;
        } else if (first != jNew) {
            first->thread = right;
        }
    } else {
        i = jOut;
        while (i->thread != iOut) {
            i = i->thread;
        }
        i->thread = right;
    }

    iOut->pred = predStem;
}

void TreeSolution::update_arc_dir(Node* jOut, Node* iOut, Node* jNew, Node* iNew,
                                  Arc *entering) {
    Node* i = iOut;

    while (i != iNew) {
        Node* j = i->pred;
        i->basic_arc = j->basic_arc;
        if (j->basic_arc_dir == ARC_UP) {
            i->basic_arc_dir = ARC_DOWN;
        } else {
            i->basic_arc_dir = ARC_UP;
        }
        i = j;
    }

    iNew->basic_arc = entering;

    if (nodes[entering->v] == iNew) {
        iNew->basic_arc_dir = ARC_UP;

    } else {
        iNew->basic_arc_dir = ARC_DOWN;
    }
}

/**
 * Updates the depth of the dynamic spanning tree structure as well as the
 * potential.
 * 
 */
void TreeSolution::update_depth_pot(Node* jOut, Node* iOut, Node* jNew, Node* iNew)
{
    Node *i, *j;

    i = jNew->thread;

    while (true) {
        j = i->pred;

        if (j == NULL) {
            // reached root
            break;
        }

        i->depth = j->depth + 1;

        // determine if there is a backward or a forward arc.
        if (i->basic_arc_dir == ARC_UP) {
            // forward arc in the tree exists
            i->potential = j->potential - i->basic_arc->cost;
        } else {
            // backward arc in the tree exists
            i->potential = j->potential + i->basic_arc->cost;
        }

        if (i->depth <= jNew->depth) {
            break;
        } else {
            i = i->thread;
        }
    }
}

long TreeSolution::solution_value() {
    long sum = 0;

    for (int i = 0; i < network->num_nodes; i++) {
        for (std::list<Arc*>::iterator it = nodes[i]->outgoing.begin();
                it != nodes[i]->outgoing.end(); ++it) {
            sum += (*it)->flow * (*it)->cost;
        }
    }
    return sum;
}


void TreeSolution::print_structure() {
    for (int i = 0; i < network->num_nodes; ++i) {
        if (nodes[i]->pred == NULL) {
            std::cout << "-1" << " ";
        } else {
            std::cout << nodes[i]->pred->id << " ";
        }
    }
    std::cout << std::endl;

    for (int i = 0; i < network->num_nodes; ++i) {
        std::cout << nodes[i]->depth << " ";
    }
    std::cout << std::endl;

    for (int i = 0; i < network->num_nodes; ++i) {
        if (nodes[i]->thread == NULL) {
            std::cout << "-1" << " ";
        } else {
            std::cout << nodes[i]->thread->id << " ";
        }
    }
    std::cout << std::endl;
}
