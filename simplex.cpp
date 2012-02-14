#include "simplex.hpp"

#include <cstdlib>
#include <list>
#include <vector>

int NWSimplex::compute_solution() {
    while (!perform_major_iteration()) continue;

    if (solution_state == SOLUTION_OPTIMAL) {
        bool has_artificial = false;
        for (int i = 0; i < network->num_nodes; ++i) {
            for (std::list<Arc*>::iterator it = network->nodes[i]->outgoing.begin();
                    it != network->nodes[i]->outgoing.end(); ++it) {
                if ((*it)->artificial && (*it)->flow > 0) {
                    has_artificial = true;
                }
            }
        }

        if (has_artificial) {
            solution_state = SOLUTION_INFEASIBLE;
        }
    }

    return solution_state;
}

bool NWSimplex::perform_major_iteration() {
    fill_candidate_list();

    if (candidate_list.empty()) {
        // no entering candidates -> already have optimal solution
        // (eventually infeasible)
        solution_state = SOLUTION_OPTIMAL;
        return true;
    }

    Arc *best = get_best_arc();
    unsigned int removed_arcs = 0;

    while (best != NULL && removed_arcs < max_min_its) {
        // arc is from L
        Cycle *cycle = compute_cycle(best);

        if (cycle->blocking == NULL) {
            std::cout << "Instance is unbounded." << std::endl;
            solution_state = SOLUTION_UNBOUNDED;
            return true;
        }

        // tree.getL().remove(best);
        best->state = ARC_STATE_T;

        tree->update(cycle->F, cycle->B, cycle->theta, best,
                cycle->blocking, cycle->common_predecessor);
        num_iterations++;

        delete cycle;

        removed_arcs++;
        best = get_best_arc();
    }

//    std::cout << "#iterations " << num_iterations << std::endl;

    return false;
}

void NWSimplex::fill_candidate_list() {
    candidate_list.clear();

    int start_node = current_startnode;
    while (candidate_list.size() < max_list_size) {
        Node *node = network->nodes[current_startnode];

        for (std::list<Arc*>::iterator it = node->outgoing.begin();
                it != node->outgoing.end(); ++it) {
            Arc *arc = (*it);
            // ignore when it is in T
            if (arc->state == ARC_STATE_T) {
                continue;
            }

            if (arc->artificial) {
                continue;
            }

            long long reduced_cost = tree->potential[arc->v]
                    - tree->potential[arc->w] + arc->cost;

            if (reduced_cost < 0 && arc->flow == 0) {
                arc->compare_value = -reduced_cost;
                candidate_list.push_back(arc);
            } else if (reduced_cost > 0 && arc->flow > 0) {
                arc->compare_value = reduced_cost;
                candidate_list.push_back(arc);
            }
        }

        ++current_startnode;

        if (current_startnode >= network->num_nodes) {
            current_startnode = 0;
        }

        if (current_startnode == start_node) {
            break;
        }
    }
}

Cycle* NWSimplex::compute_cycle(Arc* entering) {
    int predBackwards = -1;
    int predForwards = -1;
    Arc *blocking = NULL;
    bool blockingIsFromBackward = false;
    long long theta = LONG_MAX;

    std::list<Arc*> F;
    std::list<Arc*> B;

    if (entering->state == ARC_STATE_L) {
        blockingIsFromBackward = true;
        predBackwards = entering->v;
        predForwards = entering->w;
        if (entering->capacity != LONG_MAX) {
            blocking = entering;
            theta = entering->capacity - entering->flow;
        }
        F.push_back(entering);
    } else {
        blockingIsFromBackward = true;
        predBackwards = entering->w;
        predForwards = entering->v;
        blocking = entering;
        theta = entering->flow;
        B.push_back(entering);
    }

    while (predBackwards != predForwards) {

        if (tree->depth[predBackwards] > tree->depth[predForwards]) {
            int pred = tree->pred[predBackwards];
            if (tree->has_arc(predBackwards, pred)) {
                // forward arc from V to pred(V)
                Arc *arc = tree->get_arc(predBackwards, pred);
                B.push_back(arc);
                if (arc->flow < theta
                        || (arc->flow == theta && !blockingIsFromBackward)) {
                    theta = arc->flow;
                    blocking = arc;
                    blockingIsFromBackward = true;
                }
            } else {
                // backward arc from V to pred(V)
                Arc *arc = tree->get_arc(pred, predBackwards);
                F.push_back(arc);
                if (arc->capacity != LONG_MAX) {
                    long long resCapacity = arc->capacity - arc->flow;
                    if (resCapacity < theta
                            || (resCapacity == theta && !blockingIsFromBackward)) {
                        theta = resCapacity;
                        blocking = arc;
                        blockingIsFromBackward = true;
                    }
                }
            }
            predBackwards = pred;
        } else {
            int pred = tree->pred[predForwards];
            if (!tree->has_arc(predForwards, pred)) {
                // backward arc from W to pred(W)
                Arc *arc = tree->get_arc(pred, predForwards);
                B.push_back(arc);
                if (arc->flow < theta
                        || (arc->flow == theta && !blockingIsFromBackward)) {
                    theta = arc->flow;
                    blocking = arc;
                    blockingIsFromBackward = false;
                }
            } else {
                // forward arc from V to pred(V)
                Arc *arc = tree->get_arc(predForwards, pred);
                F.push_back(arc);
                if (arc->capacity != LONG_MAX) {
                    long resCapacity = arc->capacity - arc->flow;
                    if (resCapacity < theta
                            || (resCapacity == theta && !blockingIsFromBackward)) {
                        theta = resCapacity;
                        blocking = arc;
                        blockingIsFromBackward = false;
                    }
                }
            }
            predForwards = pred;
        }
    }

    if (blocking == NULL) {
        // performBestIteration relies on this ...
        solution_state = SOLUTION_UNBOUNDED;
    }

    if (theta < 0) {
        throw "Something is seriously wrong, theta=";
    }

    return new Cycle(theta, blocking, predForwards, F, B);
}

void NWSimplex::recalc_redcosts() {
    std::vector<Arc*>::iterator it = candidate_list.begin();
    while (it != candidate_list.end()) {
        Arc *arc = (*it);
        long long reducedCost = tree->potential[arc->v]
                - tree->potential[arc->w] + arc->cost;
        if (arc->flow == 0) {
            // this arc comes from L
            if (reducedCost >= 0) {
                arc->compare_value = 0;
            } else {
                arc->compare_value = -reducedCost;
            }
        } else {
            if (reducedCost <= 0) {
                arc->compare_value = 0;
            } else {
                arc->compare_value = reducedCost;
            }
        }
        ++it;
    }
}

Arc* NWSimplex::get_best_arc()
{
    Arc *best = NULL;
    long long bestRedCost = 0;

    std::vector<Arc*>::iterator it = candidate_list.begin();
    while (it != candidate_list.end()) {
        Arc *arc = (*it);

        long long reducedCost = tree->potential[arc->v]
                - tree->potential[arc->w] + arc->cost;

        if (arc->flow == 0) {
            // this arc comes from L
            if (reducedCost >= 0) {
                arc->compare_value = 0;
            } else {
                arc->compare_value = -reducedCost;
            }
        } else {
            if (reducedCost <= 0) {
                arc->compare_value = 0;
            } else {
                arc->compare_value = reducedCost;
            }
        }

        if (labs(arc->compare_value) > bestRedCost) {
            best = arc;
            bestRedCost = labs(arc->compare_value);
        }
        ++it;
    }
    return best;
}

long long NWSimplex::solution_value()
{
    return tree->solution_value();
}

bool arc_compare (Arc* i, Arc* j)
{
    if (i->v == j->w) {
        return i->w <= j->w;
    } else {
        return i->v <= j->v;
    }
}

std::list<Arc*>* NWSimplex::sorted_solution_arcs() {
    std::list<Arc*> *result = new std::list<Arc*>();
    for (int i = 0; i < network->num_nodes; ++i) {
        for (std::list<Arc*>::iterator it = network->nodes[i]->outgoing.begin();
                it != network->nodes[i]->outgoing.end(); ++it) {
            Arc *arc = (*it);
            if (arc->flow > 0) {
               result->push_back(arc);
            } 
        }
    }
    return result;
}
