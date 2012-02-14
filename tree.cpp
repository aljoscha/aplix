#include "tree.hpp"

#include <stack>

void TreeSolution::determine_initial_tree() {
    long long demand_sum = 0;
    for (int i = 0; i < network->num_nodes; ++i) {
        demand_sum += network->nodes[i]->demand;
    }

    if (demand_sum != 0) {
        throw "Network is infeasible because demands do not sum up to zero.";
    }

    for (int i = 1; i < network->num_nodes; ++i) {
        Node *node = network->nodes[i];

        Arc *arc = NULL;
        if (node->demand < 0) {
            // this means that node has DEMAND!! eg we want to make an arc
            // INTO this node
            arc = network->add_artificial_arc(ROOT_NODE, node->id);
            arc->flow = -node->demand;
        } else {
            // network.getDemand(node) >= 0
            // eg node has supply or flow conservation! want to make an arc
            // FROM this node
            arc = network->add_artificial_arc(node->id, ROOT_NODE);
            arc->flow = node->demand;
        }
        arc->state = ARC_STATE_T;
        treearcs[arc->v][arc->w] = arc;
    }
}

void TreeSolution::calc_initial_tree_structure() {
    depth[ROOT_NODE] = 0;
    pred[ROOT_NODE] = -1;
    thread[ROOT_NODE] = 1;
    potential[ROOT_NODE] = 0;		

    int num_treearcs = network->num_nodes;
    // spanning tree has num_nodes -1 nodes
    // set thread of last node to 0
    pred[num_treearcs - 1] = 0;
    thread[num_treearcs - 1] = 0;
    depth[num_treearcs - 1] = 1;
    if (treearcs[0][num_treearcs - 1] != NULL) {
        potential[num_treearcs - 1] = network->max_cost;
    } else {
        potential[num_treearcs - 1] = -network->max_cost;
    }

    for (int i = 1; i < num_treearcs - 1; ++i) {
        depth[i] = 1;
        pred[i] = 0;
        thread[i] = i + 1;
        if (treearcs[0][i] != NULL) {
            potential[i] = network->max_cost;
        } else {
            potential[i] = -network->max_cost;
        }
    }
}

void TreeSolution::update(std::list<Arc*> F, std::list<Arc*> B,
                long long theta,
                Arc *entering,
                Arc* leaving,
                int common_predecessor) {

    if (theta != 0) {
        for (std::list<Arc*>::iterator it = F.begin(); it != F.end(); ++it) {
            (*it)->flow += theta;
        }

        for (std::list<Arc*>::iterator it = B.begin(); it != B.end(); ++it) {
            (*it)->flow -= theta;
        }
    }

    if (entering != leaving) {
        treearcs[leaving->v][leaving->w] = NULL;
    }

    if (leaving->flow == 0) {
        // must now be in L
        if (!leaving->artificial) {
            leaving->state = ARC_STATE_L;
        } else {
            network->nodes[leaving->v]->outgoing.remove(leaving);
        }
    } else {
        // must be at max capacity, so add to U
        leaving->state = ARC_STATE_U;
    }

    // entering arc must not be removed from L/U because getEnteringArcFrom*
    // already do that
    entering->state = ARC_STATE_T;

    if (entering != leaving) {
        treearcs[entering->v][entering->w] = entering;
    }

    this->update_tree(entering, leaving, common_predecessor);
}

void TreeSolution::update_tree(Arc* entering, Arc* leaving, int join)
{
    if (entering == leaving) {
        return;
    }

    int jOut, iOut, jNew = -1, iNew = -1;
    // first determine iOut and jOut
    if (pred[leaving->v] == leaving->w) {
        jOut = leaving->w;
        iOut = leaving->v;
    } else if (pred[leaving->w] == leaving->v) {
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
    while (depth[i] > depth[join]) {
        if (i == leaving->v || i == leaving->w) {
            iNew = entering->w;
            jNew = entering->v;
            break;
        }
        i = pred[i];
    }
    i = entering->v;
    while (depth[i] > depth[join]) {
        if (i == leaving->v || i == leaving->w) {
            iNew = entering->v;
            jNew = entering->w;
            break;
        }
        i = pred[i];
    }

    if (jNew == -1) {
        throw "Serious problem.";
    }

    update_thread_parent(jOut, iOut, jNew, iNew, join);
    update_depth_pot(jOut, iOut, jNew, iNew);
}

void TreeSolution::update_thread_parent(int jOut, int iOut, int jNew, int iNew,
        int join) {
    bool parent_first = false;
    int i;
    int first = -1;
    int last;
    int right;

    int stem, newStem, predStem;

    if (join == jOut) {
        // determine whether jNew or iNew appear first on the thread
        i = pred[join];

        // have to do this because ROOT_NODE might be the "join"
        if (i == -1) {
            for (int index = 0; index < network->num_nodes; ++index) {
                if (thread[index] == 0) {
                    i = index;
                }
            }

        }

        while (thread[i] != iNew && thread[i] != jNew) {
            i = thread[i];
        }

        if (thread[i] == jNew) {
            parent_first = true;
        }

        while (thread[i] != iOut) {
            i = thread[i];
        }
        first = i;
    }

    // now traverse the stem
    i = iNew;

    // calculate the last right successor for the first iteration
    while (depth[thread[i]] > depth[iNew]) {
        i = thread[i];
    }
    right = thread[i];
    // i is the last right successor if i (iNew) now

    if (thread[jNew] == iOut) {
        last = i;
        while (depth[last] > depth[iOut]) {
            last = thread[last];
        }
        if (last == iOut) {
            last = thread[last];
        }
    } else {
        last = thread[jNew];
    }

    thread[jNew] = iNew;
    stem = iNew;
    predStem = jNew;

    while (stem != iOut) {
        // i is the last right successor at this point
        thread[i] = pred[stem];

        i = pred[stem];
        while (thread[i] != stem) {
            i = thread[i];
        }
        // i is the last left successor of parent
        thread[i] = right;

        newStem = pred[stem];
        pred[stem] = predStem;
        predStem = stem;
        stem = newStem;

        // set i to the last right successor again
        i = stem;
        while (depth[thread[i]] > depth[stem]) {
            i = thread[i];
        }
        right = thread[i];
    }

    thread[i] = last;

    if (join == jOut) {
        if (!parent_first) {
            i = jOut;
            while (thread[i] != iOut) {
                i = thread[i];
            }
            thread[i] = right;
        } else if (first != jNew) {
            thread[first] = right;
        }
    } else {
        i = jOut;
        while (thread[i] != iOut) {
            i = thread[i];
        }
        thread[i] = right;
    }

    pred[iOut] = predStem;
}

/**
 * Updates the depth of the dynamic spanning tree structure as well as the
 * potential.
 * 
 */
void TreeSolution::update_depth_pot(int jOut, int iOut, int jNew, int iNew)
{
    int i, j;

    i = thread[jNew];

    while (true) {
        j = pred[i];

        if (j == -1) {
            // reached root
            break;
        }

        depth[i] = depth[j] + 1;

        // determine if there is a backward or a forward arc.
        if (treearcs[i][pred[i]] != NULL) {
            // forward arc in the tree exists
            potential[i] = potential[pred[i]]
                    - treearcs[i][pred[i]]->cost;
        } else if (treearcs[pred[i]][i] != NULL) {
            // backward arc in the tree exists
            potential[i] = potential[pred[i]]
                    + treearcs[pred[i]][i]->cost;
        } else {
            throw "Update potential cumputation failed from {} to {}";
        }

        if (depth[i] <= depth[jNew]) {
            break;
        } else {
            i = thread[i];
        }
    }
}

bool TreeSolution::has_arc(int from, int to) {
    return treearcs[from][to] != NULL;
}

Arc* TreeSolution::get_arc(int from, int to) {
    return treearcs[from][to];
}

long long TreeSolution::solution_value() {
    long long sum = 0;

    for (int i = 0; i < network->num_nodes; i++) {
        for (std::list<Arc*>::iterator it = network->nodes[i]->outgoing.begin();
                it != network->nodes[i]->outgoing.end(); ++it) {
            sum += (*it)->flow * (*it)->cost;
        }
    }
    return sum;
}

void TreeSolution::print_structure() {
    for (int i = 0; i < network->num_nodes; ++i) {
        std::cout << pred[i] << " ";
    }
    std::cout << std::endl;

    for (int i = 0; i < network->num_nodes; ++i) {
        std::cout << depth[i] << " ";
    }
    std::cout << std::endl;

    for (int i = 0; i < network->num_nodes; ++i) {
        std::cout << thread[i] << " ";
    }
    std::cout << std::endl;
}
