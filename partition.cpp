#include "partition.h"
#include "circuit.h"
#include "spdlog/spdlog.h"
#include <queue>
#include <vector>
#include <list>

// the decision tree just exists,
// any given node in the tree represents a partial or complete set of decisions


a3::partition::partition(circuit* c) {
    circ = c;
    unassigned = vector<cell*>(c->get_cells()); 
    vl = vector<cell*>();
    vr = vector<cell*>();
}

bool a3::partition::assign(vector<cell*>& v, cell* c) {
    bool ret = false;
    auto pos = std::find(unassigned.begin(), unassigned.end(), c);
    if (pos != unassigned.end()) {  // if we actually found it in the unassigned list
        unassigned.erase(pos);
        if (std::find(v.begin(), v.end(), c) == v.end()) {
            v.push_back(c);
            ret = true;
        } else {
            spdlog::warn("cell {} was already in list", c->label);
        }
    } else {
        spdlog::warn("cell {} was not in unassigned list", c->label);
    }
    return ret;
}

bool a3::partition::assign_left(cell* c) {
    return assign(vl, c);
}

bool a3::partition::assign_right(cell* c) {
    return assign(vr, c);
}

cell* a3::partition::make_left_supercell() {
    return new cell(vl);
}

cell* a3::partition::make_right_supercell() {
    return new cell(vr);
}


int a3::partition::cost() {
    // iterate over every net.  See if it has cells in both left and right
    int cost = 0;
    for (auto& n: circ->get_nets()) {
        // n.first is the string label
        // n.second is the net object
        bool found_in_left = false;
        bool found_in_right = false;
        for (auto& cl : n.second->get_cell_labels()) {
            cell* c = circ->get_cell(cl);
            if (std::find(vl.begin(), vl.end(), c) != vl.end()) {
                found_in_left = true;
            }
            else if (std::find(vr.begin(), vr.end(), c) != vr.end()) {
                found_in_right = true;
            }
            if (found_in_left && found_in_right) {
                spdlog::debug("net {} is in the cut set", n.first);
                cost++;
                break;
            }
        }
    }
    return cost;
}

// lower bound function
// right now - just use the number of cut nets
int a3::partition::lb() {
    return cost();
}

bool cell_sort_most_nets(cell* a, cell* b) {
    return a->get_net_labels().size() > b->get_net_labels().size();
}

cell* g_supercell;
bool sort_by_most_mutual_to_g_supercell(cell* a, cell* b) {
    return g_supercell->get_mutual_net_labels(a).size() > g_supercell->get_mutual_net_labels(b).size();
}

a3::partition* a3::partition::initial_solution() {
    // sort cells by their fanout
    // pick the highest fanout node, put it on the left (cell 1)
    // among the next highest cells, pick the one with the least mutual overlap with cell 1.  put it on the right.
    // alternate left and right, picking the next node via one with the most mutual nets to the supernode of that side


    a3::partition* p = new a3::partition(circ);
    std::sort(p->unassigned.begin(), p->unassigned.end(), cell_sort_most_nets);

    cell* lcell = *unassigned.begin();
    p->assign_left(*unassigned.begin());

    cell* rcell = *(unassigned.begin());

    // for the right, we want a cell that has the least overlap
    // but still has many connections
    int score = 0;
    for(auto& c : p->unassigned) {
        int new_score = 0;
        int n_mutual_nets = lcell->get_mutual_net_labels(c).size();
        int n_r_nets = c->get_net_labels().size();
        int n_l_nets = lcell->get_net_labels().size();
        int n_unique_r_nets = n_r_nets - n_mutual_nets;
        int n_unique_l_nets = n_l_nets - n_mutual_nets;

        new_score = (n_unique_r_nets)*(n_unique_l_nets);
        if (new_score > score) {
            score = new_score;
            rcell = c;
        }
    }
    p->assign_right(rcell);

    bool insert_right = false;
    while (p->unassigned.size() > 0) {
        g_supercell = insert_right ? p->make_right_supercell() : p->make_left_supercell();
        std::sort(p->unassigned.begin(), p->unassigned.end(), sort_by_most_mutual_to_g_supercell);

        cell* c = *p->unassigned.begin();
        bool rc = insert_right ? p->assign_right(c) : p->assign_left(c);
        if (!rc) {
            spdlog::warn("issue inserting cell {} in {}", c->label, insert_right? "right" : "left");
        } else {
            spdlog::info("inserted cell {} in {}", c->label, insert_right? "right" : "left");
        }

        insert_right = !insert_right;
        delete g_supercell;
    }
    return p;
}
