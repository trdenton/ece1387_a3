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

struct supercell {
    
};

bool cell_sort_most_nets(cell* a, cell* b) {
    return a->get_net_labels().size() > b->get_net_labels().size();
}

a3::partition* a3::partition::initial_solution() {
    // sort cells by their fanout
    // pick the highest fanout node, put it on the left (cell 1)
    // among the next highest cells, pick the one with the least mutual overlap with cell 1.  put it on the right.
    // alternate left and right, picking the next node via one with the most mutual nets to the supernode of that side

    list<cell*> cells;
    std::copy(circ->get_cells().begin(), circ->get_cells().end(), std::back_inserter(cells));
    cells.sort(cell_sort_most_nets);

    a3::partition* p = new a3::partition(circ);

    p->assign_left(*cells.begin());
    cells.erase(cells.begin());

    bool insert_right = true;
    while (cells.size() > 0) {
        cell* c;
        insert_right ? p->assign_right(c) : p->assign_left(c);
        insert_right = !insert_right;
    }
    return p;
}
