#include "partition.h"
#include "circuit.h"
#include "spdlog/spdlog.h"
#include <queue>
#include <vector>
#include <algorithm>
#include <numeric>
#include <list>

// the decision tree just exists,
// any given node in the tree represents a partial or complete set of decisions

bool cell_sort_most_nets(cell* a, cell* b);
bool sort_by_most_mutual_to_g_supercell(cell* a, cell* b);

a3::partition::partition(circuit* c) {
    circ = c;
    unassigned = vector<cell*>(c->get_cells()); 
    std::sort(unassigned.begin(),unassigned.end(),cell_sort_most_nets);
    vl = vector<cell*>();
    vr = vector<cell*>();
    //spdlog::debug("new partition: {}", to_string());


    // initially, all nets are uncut
    for(auto c : c->get_cells()) {
        cell_uncut_nets[c->label] = vector<int>(c->get_net_labels());
    }
}

string a3::partition::to_string()
{
        std::ostringstream os;

        os << "[part][u ";
        for (auto u : unassigned)
            os << u->label << ", ";
        os << "][l ";
        for (auto l : vl)
            os << l->label << ", ";
        os << "][r ";
        for (auto r : vr)
            os << r->label << ", ";
        os << "]";

        return os.str();
};

int a3::partition::cost() {
    return cut_nets.size;
}

a3::partition* a3::partition::copy() {
    a3::partition* p = new a3::partition(circ);
    p->unassigned = vector<cell*>(unassigned);
    p->vl = vector<cell*>(vl);
    p->vr = vector<cell*>(vr);
    p->cut_nets = bitfield(&cut_nets);
    p->cell_uncut_nets = map<string, vector<int>>(cell_uncut_nets);
    return p;
}

bool a3::partition::assign(vector<cell*>& v, cell* c) {
    bool ret = false;
    auto pos = std::find(unassigned.begin(), unassigned.end(), c);
    if (pos != unassigned.end()) {  // if we actually found it in the unassigned list
        if (std::find(v.begin(), v.end(), c) == v.end()) {
            v.push_back(c);
            unassigned.erase(pos);
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
    bool ret = assign(vl, c);
    if (ret) {
        cut_nets_from_adding_cell(vl, c);
    }
    return ret;
}

bool a3::partition::assign_right(cell* c) {
    bool ret = assign(vr, c);
    if (ret) {
        cut_nets_from_adding_cell(vr, c);
    }
    return ret;
}

bool a3::partition::_unassign(vector<cell*>& v, cell* c) {
    bool rc = false;
    auto elem = std::find(v.begin(), v.end(), c);
    if (elem != v.end()) {
        rc = true; 
        v.erase(elem);
    }
    return rc;
}

bool a3::partition::unassign(cell* c) {
    bool rc = false;

    if (_unassign(vl, c)) {
        rc = true;
    } else if (_unassign(vr, c)) {
        rc = true;
    }

    // insertion sort to maintain unassigned order
    if (rc) {
        for(auto iter = unassigned.begin(); iter < unassigned.end(); ++iter) {
            if (cell_sort_most_nets(c, *iter)) {
                unassigned.insert(iter, c);
                return true;
            }
        }
        unassigned.push_back(c);
        
    }
    return rc;
}

cell* a3::partition::make_left_supercell() {
    return new cell(vl);
}

cell* a3::partition::make_right_supercell() {
    return new cell(vr);
}

void a3::partition::cut_nets_from_adding_cell(vector<cell*> v, cell* c) {
    // project what incremental cost is for a left, right addition
    vector<cell*> v_other = (v == vl ? vr : vl);

    // average 16 nets per cell
    // and ~30+ per circuit unique nets
    // BUT we only need to loop over uncut nets, and this shrinks 
    // we can track each cell's cut nets?  is that practical? how do we broadcast to each cell when its net is cut by another cell
    // do we get the intersection of cell labels and uncut labels?
    // how many cells per net?
    // in cct3... idk its between like 4 and 16.  call it 12.
    // so looping over all a cells nets, and then all of each nets cells, thats 16*12 = 192 per cell

    vector<int> uncut_nets = cell_uncut_nets[c->label];
    vector<int>::iterator n = uncut_nets.begin();
    stack<vector<int>::iterator> to_delete;// = vector<vector<string>::iterator>();
    for (; n < uncut_nets.end(); ++n) {
        if (cut_nets.get(*n))
            continue;

        for (auto& cl : circ->get_net(*n)->get_cell_labels()) {
            cell* net_c = circ->get_cell(cl);
            if (net_c == c)
                continue;

            if (std::find(v_other.begin(), v_other.end(), net_c) != v_other.end()) {
                cut_nets.set(*n); // can only cut once
                to_delete.push(n);
                break;
            }
        }
    }
    while(!to_delete.empty()) {
        vector<int>::iterator del = to_delete.top();
        to_delete.pop();
        uncut_nets.erase(del);
    }
    cell_uncut_nets[c->label] = uncut_nets;
}

/*
MODIFIES STATE

changes the current cut set
changes the current cost
*/
int a3::partition::calculate_cut_set() {
    // iterate over every net.  See if it has cells in both left and right
    cut_nets = bitfield();
    for (auto& n: circ->get_nets()) {
        // if weve already cut a net, you cant cut it more...
        if (cut_nets.get(n->label))
            continue;
        // n.first is the string label
        // n.second is the net object
        bool found_in_left = false;
        bool found_in_right = false;

        // for each net (above), see if we have a cell in each half
        // if thats the case, weve cut the net
        for (auto& cl : n->get_cell_labels()) {
            cell* c = circ->get_cell(cl);
            if (std::find(vl.begin(), vl.end(), c) != vl.end()) {
                found_in_left = true;
            }
            else if (std::find(vr.begin(), vr.end(), c) != vr.end()) {
                found_in_right = true;
            }
            if (found_in_left && found_in_right) {
                //spdlog::debug("net {} is in the cut set", n.first);
                cut_nets.set(n->label);
                // proceed to next net
                break;
            }
        }
    }
    return cut_nets.size;
}

// lower bound function
// right now - just use the number of cut nets
int a3::partition::lb() {
    return cost();
}

bool cell_sort_most_nets(cell* a, cell* b) {
    return a->get_num_nets() > b->get_num_nets();
}

cell* g_supercell;
bool sort_by_most_mutual_to_g_supercell(cell* a, cell* b) {
    return g_supercell->get_mutual_net_labels(a).size() > g_supercell->get_mutual_net_labels(b).size();
}

void a3::partition::initial_solution() {
    // sort cells by their fanout
    // pick the highest fanout node, put it on the left (cell 1)
    // among the next highest cells, pick the one with the least mutual overlap with cell 1.  put it on the right.
    // alternate left and right, picking the next node via one with the most mutual nets to the supernode of that side


    //a3::partition* p = new a3::partition(circ);
    std::sort(unassigned.begin(), unassigned.end(), cell_sort_most_nets);

    cell* lcell = *unassigned.begin();
    assign_left(lcell);

    cell* rcell = *(unassigned.begin());

    // for the right, we want a cell that has the least overlap
    // but still has many connections
    int score = 0;
    for(auto& c : unassigned) {
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
    assign_right(rcell);

    bool insert_right = false;
    while (unassigned.size() > 0) {
        g_supercell = insert_right ? make_right_supercell() : make_left_supercell();
        std::sort(unassigned.begin(), unassigned.end(), sort_by_most_mutual_to_g_supercell);

        cell* c = *unassigned.begin();
        bool rc = insert_right ? assign_right(c) : assign_left(c);
        if (!rc) {
            spdlog::warn("issue inserting cell {} in {}", c->label, insert_right? "right" : "left");
        } else {
            spdlog::debug("inserted cell {} in {}", c->label, insert_right? "right" : "left");
        }

        insert_right = !insert_right;
        delete g_supercell;
    }
    assert(vl.size() == vr.size());
    calculate_cut_set();
}

void a3::partition::initial_solution_random() {
    bool insert_right = true;
    srand(time(NULL));
    while(unassigned.size() > 0) {
        int random_index = rand() % unassigned.size();
        cell* c = unassigned[random_index];
        
        bool rc = insert_right ? assign_right(c) : assign_left(c);
        if (!rc) {
            spdlog::warn("issue inserting cell {} in {}", c->label, insert_right? "right" : "left");
        } else {
            spdlog::debug("inserted cell {} in {}", c->label, insert_right? "right" : "left");
        }

        insert_right = !insert_right;
    }
    spdlog::debug("random assigned cost: {}", cost());
    calculate_cut_set();
    spdlog::debug("...and recalculating: {}", cost());
}

void a3::partition::print_cut_nets() {
    spdlog::debug("[");
    /*
    for(auto &n : cut_nets) {
        spdlog::debug("{}", n);
    }
    */
    spdlog::debug("]");
}

pnode* traverser::bfs_step() {
    pnode* rc = nullptr;
    if (!q_bfs.empty()) {
        pnode* pn = q_bfs.front(); q_bfs.pop();


        visited_nodes++;
        if (pn->p->unassigned.size() > 0) {

            // explore putting it on the left
            if (!prune_imbalance || (pn->p->vl.size() < cells.size()/2 )) {
                pn->left = new pnode();
                
                // UI drawing related
                pn->left->level = pn->level + 1;
                pn->left->y = pn->y + 64.0*PNODE_DIAMETER;
                int levels_to_leaf = cells.size() - pn->level -2;
                pn->left->x = pn->x - PNODE_DIAMETER*(2<<levels_to_leaf);

                pn->left->parent = pn;
                pn->left->p = pn->p->copy();
                pn->left->p->assign_left(*(pn->p->unassigned.begin())); //unassigned.begin
                if (!prune_lb || !prune(pn->left->p, best)) {
                    q_bfs.push(pn->left);
                    pnodes.push_back(pn->left);
                }
                if (pn->left->p->cost() > circ->get_n_nets()) {
                    spdlog::error("cost has exceeded num nets...");
                    pn->left->p->print_cut_nets();
                }
            } else {
                spdlog::debug("pruning: imbalance");
            }

            // explore putting it on the right
            if ( (pn->level > 0 || !prune_symmetry) && (!prune_imbalance || (pn->p->vr.size() < cells.size()/2 ))) {
                pn->right = new pnode();
                
                // UI drawing related
                pn->right->level = pn->level + 1;
                pn->right->y = pn->y + 64.0*PNODE_DIAMETER;
                int levels_to_leaf = cells.size() - pn->level -2;
                pn->right->x = pn->x + PNODE_DIAMETER*(2<<levels_to_leaf);

                pn->right->parent = pn;
                pn->right->p = pn->p->copy();
                pn->right->p->assign_right(*(pn->p->unassigned.begin())); //unassigned.begin
                if (!prune_lb || !prune(pn->right->p, best)) {
                    q_bfs.push(pn->right);
                    pnodes.push_back(pn->right);
                }
                if (pn->right->p->cost() > circ->get_n_nets()) {
                    spdlog::error("cost has exceeded num nets...");
                    pn->right->p->print_cut_nets();
                }
            } else {
                spdlog::debug("pruning: imbalance");
            }

        } else {
            spdlog::info("leaf node: {}", pn->p->cost());
            int tcost = pn->p->cost();
            int bcost = (*best)->cost();
            prune(pn->p, best);
            spdlog::info("Test cost/recalc: {}/{} best cost/recalc: {}/{}", tcost, pn->p->calculate_cut_set(), bcost, (*best)->calculate_cut_set());
        }

        rc = pn;
    } 
    return rc;
}

traverser::traverser(circuit* c, a3::partition** _best, bool (*prune_fn)(a3::partition* test, a3::partition** best)) {
    cells = vector<cell*>(c->get_cells());
    circ = c;
    std::sort(cells.begin(), cells.end(), cell_sort_most_nets);
    visited_nodes = 0;

    // only turn this off for test mode
    prune_imbalance = true;
    prune_lb = true;

    root = new pnode();
    root->parent = nullptr;
    root->level = 0;
    root->y = PNODE_DIAMETER/2.0;
    root->x = circ->get_display_width()/2.0;
    root->p = new a3::partition(c);

    root->p->calculate_cut_set();
    
    q_bfs = queue<pnode*>();
    q_bfs.push(root);

    prune = prune_fn;
    best = _best;
    pnodes.push_back(root);

}

traverser::~traverser() {
    del_tree(root);
}

void del_tree(pnode* root) {
    stack<pnode*> s;
    s.push(root);
    while(!s.empty()) {
        pnode* pn = s.top();
        if (pn->left != nullptr or pn->right != nullptr) {
            if (pn->left != nullptr) {
                s.push(pn->left);
            } 
            if (pn->right != nullptr) {
                s.push(pn->right);
            }
        } else {
            if (pn->parent != nullptr) {
                if (pn->parent->left == pn) {
                    pn->parent->left = nullptr;
                } else if (pn->parent->right == pn) {
                    pn->parent->right = nullptr;
                } 
            }
            s.pop();
            delete pn->p;
            delete pn;
        }
    }
}

// returning true means we prune the tree at (test) and below
bool prune_basic_cost(a3::partition* test, a3::partition** best) {
    bool ret = false;

    spdlog::debug("\t({} vs {}) [{}]", test->cost(), (*best)->cost(), test->unassigned.size());
    if (test->cost() < (*best)->cost()) {
        if (test->unassigned.size() == 0) {
            spdlog::info("found new best! ({} < {}) {}", test->cost(), (*best)->cost(), (void*)test);
            *best = test;
        }
    } else {
        spdlog::debug("PRUNING ({} >= {})", test->cost(), (*best)->cost());
        ret = true;
    }
    return ret;
}
