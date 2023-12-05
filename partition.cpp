#include "partition.h"
#include "bitfield.h"
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

a3::partition::partition() {
}

a3::partition::partition(circuit* c) {
    circ = c;
    //spdlog::debug("new partition: {}", to_string());


    // initially, all nets are uncut
    for(auto nl : circ->get_nets()) {
        uncut_nets.set(nl->label);
    }
    for(auto cl : circ->get_cells()) {
        unassigned_cells.set(cl->label);
    }
}

a3::partition::partition(a3::partition* other) {
    circ = other->circ;
    vr_cells = other->vr_cells;
    vl_cells = other->vl_cells;
    vr_nets = other->vr_nets;
    vl_nets = other->vl_nets;
    unassigned_cells = other->unassigned_cells;
    uncut_nets = other->uncut_nets;
    cut_nets = other->cut_nets;
}

string a3::partition::to_string()
{

        std::ostringstream os;

        os << "[part][u ";
        for (auto u : unassigned_cells.to_vec())
            os << u << ", ";
        os << "][l ";
        for (auto l : vl_cells.to_vec())
            os << l << ", ";
        os << "][r ";
        for (auto r : vr_cells.to_vec())
            os << r << ", ";
        os << "]";

        return os.str();
};

int a3::partition::cost() {
    return cut_nets.size;
}

void a3::partition::update_cut_nets() {
    for (auto nl : uncut_nets.to_vec()) {
        if (vr_nets.get(nl) && vl_nets.get(nl)) {
            uncut_nets.clear(nl);
            cut_nets.set(nl);
        }
    }
}

void a3::partition::assign_left(cell* c) {
    unassigned_cells.clear(c->label);
    vl_cells.set(c->label);
    for(auto nl : c->net_labels.to_vec()) {
        vl_nets.set(nl);
    }
    update_cut_nets();
}

void a3::partition::assign_right(cell* c) {
    unassigned_cells.clear(c->label);
    vr_cells.set(c->label);
    for(auto nl : c->net_labels.to_vec()) {
        vr_nets.set(nl);
    }
    update_cut_nets();
}



// lower bound function
// right now - just use the number of cut nets
int a3::partition::lb() {
    return cost();
}

bool cell_sort_most_nets(cell* a, cell* b) {
    return a->net_labels.size > b->net_labels.size;
}


void a3::partition::initial_solution() {
    initial_solution_random();
    for(int i = 0; i < 1000; ++i) {
        a3::partition rand = a3::partition(this);

        rand.initial_solution_random();
        if (rand.cut_nets.size < cut_nets.size) {
            vr_cells = rand.vr_cells;
            vl_cells = rand.vl_cells;
            vr_nets = rand.vr_nets;
            vl_nets = rand.vl_nets;
            unassigned_cells = rand.unassigned_cells;
            uncut_nets = rand.uncut_nets;
            cut_nets = rand.cut_nets;
        }
    }
}

void a3::partition::initial_solution_random() {
    bool insert_right = true;
    srand(time(NULL));
    vector<cell*> unassigned = circ->get_cells();

    vr_cells = bitfield();
    vl_cells = bitfield();
    vr_nets = bitfield();
    vl_nets = bitfield();
    cut_nets = bitfield();
    unassigned_cells = bitfield();
    uncut_nets = bitfield();

    for(auto nl : circ->get_nets()) {
        uncut_nets.set(nl->label);
    }

    for(auto cl : circ->get_cells()) {
        unassigned_cells.set(cl->label);
    }

    while(!unassigned.empty()) {
        int random_index = rand() % unassigned.size();
        cell* c = unassigned[random_index];

        insert_right ? assign_right(c) : assign_left(c);

        insert_right = !insert_right;
        unassigned.erase(unassigned.begin() + random_index);
    }
}

cell* a3::partition::next_unassigned(vector<cell*> cell_prio_list) {
    return cell_prio_list[vr_cells.size + vl_cells.size];
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

pnode::pnode() {
}

pnode* traverser::bfs_step() {
    pnode* rc = nullptr;
    if (!q_bfs.empty()) {
        pnode* pn = q_bfs.front(); q_bfs.pop();


        visited_nodes++;
        if (pn->p.unassigned_cells.size > 0) {

            // explore putting it on the left
            if (!prune_imbalance || (pn->p.vl_cells.size < cells.size()/2 )) {
                pn->left = new pnode();

                // UI drawing related
                pn->left->level = pn->level + 1;
                pn->left->y = pn->y + 64.0*PNODE_DIAMETER;
                int levels_to_leaf = cells.size() - pn->level -2;
                pn->left->x = pn->x - PNODE_DIAMETER*(2<<levels_to_leaf);

                pn->left->parent = pn;
                pn->left->p = a3::partition(pn->p);
                pn->left->p.assign_left( pn->p.next_unassigned(cells) );
                if (!prune_lb || !prune(&pn->left->p, best)) {
                    q_bfs.push(pn->left);
                    pnodes.push_back(pn->left);
                } else {
                    delete pn->left;
                    pn->left = nullptr;
                }
            } else {
                spdlog::debug("pruning: imbalance");
            }

            // explore putting it on the right
            if ( (pn->level > 0 || !prune_symmetry) && (!prune_imbalance || (pn->p.vr_cells.size < cells.size()/2 ))) {
                pn->right = new pnode();
                
                // UI drawing related
                pn->right->level = pn->level + 1;
                pn->right->y = pn->y + 64.0*PNODE_DIAMETER;
                int levels_to_leaf = cells.size() - pn->level -2;
                pn->right->x = pn->x + PNODE_DIAMETER*(2<<levels_to_leaf);

                pn->right->parent = pn;
                pn->right->p = a3::partition(pn->p);
                pn->right->p.assign_right(pn->p.next_unassigned(cells));
                if (!prune_lb || !prune(&pn->right->p, best)) {
                    q_bfs.push(pn->right);
                    pnodes.push_back(pn->right);
                } else {
                    delete pn->right;
                    pn->right = nullptr;
                }
            } else {
                spdlog::debug("pruning: imbalance");
            }
        } else { 
            spdlog::info("leaf node: {}", pn->p.cost());
            int tcost = pn->p.cost();
            int bcost = (*best)->cost();
            prune(&pn->p, best);
        }
        rc = pn;
    } 
    return rc;
}

traverser::traverser(circuit* c, a3::partition** _best, bool (*prune_fn)(a3::partition* test, a3::partition** best)) {
    cells = vector<cell*>(c->get_cells());
    circ = c;
    std::sort(cells.begin(), cells.end(), cell_sort_most_nets);
    cur_cell = cells.begin();
    visited_nodes = 0;

    // only turn this off for test mode
    prune_imbalance = true;
    prune_lb = true;

    root = new pnode();
    root->parent = nullptr;
    root->level = 0;
    root->y = PNODE_DIAMETER/2.0;
    root->x = circ->get_display_width()/2.0;
    root->p = a3::partition(c);

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
            delete pn;
        }
    }
}

bitfield a3::partition::num_guaranteed_cut_nets() {
    // foreach uncut net - if the number of unassigned cells is > half, its a guaranteed cut
    bitfield ret;
    int cut_net_count = 0;
    map<int,int> net_cell_counts;

    for(auto n : uncut_nets.to_vec()) {
        net_cell_counts[n] = 0;
    }

    int min_size = circ->get_n_cells()/2;
    for(auto cl : unassigned_cells.to_vec()) {
        for (auto nl : circ->get_cell(cl)->net_labels.to_vec()) {
            if (uncut_nets.get(nl)) {
                net_cell_counts[nl]++;
            }
        }
    }

    for (auto n : uncut_nets.to_vec()) {
        if (net_cell_counts[n] > min_size) {
            cut_net_count++;
            ret.set(n);
        }
    }

    return ret;
}

bitfield a3::partition::one_partition_full_cut_nets() {
    bitfield ret;
    bitfield *side = nullptr;

    if (vl_cells.size == circ->get_n_cells()/2) {
        side = &vl_cells;
    } else if (vr_cells.size == circ->get_n_cells()/2) {
        side = &vr_cells;
    }

    if (nullptr != side) {
        for(auto cl : unassigned_cells.to_vec()) {
            for (auto nl : uncut_nets.to_vec()) {
                if (circ->get_cell(cl)->net_labels.get(nl)) {
                    if (side->get(nl)) {
                        ret.set(nl);
                    }
                }
            }
        }
    }

    return ret;
}

bitfield a3::partition::min_number_anchored_nets_cut() {
    bitfield bl;
    bitfield br;
    for (auto cl : unassigned_cells.to_vec()) {
        cell* c = circ->get_cell(cl); 
        vector<int> myleftnets, myrightnets;
        for (auto nl : c->net_labels.to_vec()) {
            int num_added=0;
            if (vl_nets.get(nl) && !vr_nets.get(nl)) {
                myleftnets.push_back(nl);
                num_added++;
            }
            if (vr_nets.get(nl) && !vl_nets.get(nl)) {
                myrightnets.push_back(nl);
                num_added++;
            }
            if (num_added == 2){ 
                spdlog::warn("net {} should have been cut...", nl);
            }
        }
        if (myleftnets.size() > 0 && myrightnets.size() > 0) {
            if (myleftnets.size() < myrightnets.size()) {
                for(auto nl : myleftnets) {
                    bl.set(nl);
                }
            } else {
                for(auto nl : myrightnets) {
                    br.set(nl);
                }
            }
        }
    }
    // need intersection of the above, cant double count...
    bitfield bu = bl.union_with(br);
    return bu;
}

// returning true means we prune the tree at (test) and below
bool prune_basic_cost(a3::partition* test, a3::partition** best) {
    bool ret = false;

    // these are different measures, there can be overlap, so cant apply both
    bitfield guaranteed_cuts = test->num_guaranteed_cut_nets();
    bitfield anchored_cuts = test->min_number_anchored_nets_cut();
    bitfield full_partition_cuts = test->one_partition_full_cut_nets();
    int min_added_cuts = 0;//guaranteed_cuts.union_with(anchored_cuts).union_with(full_partition_cuts).size;

    spdlog::debug("\t({} vs {}) [{}]", test->cost(), (*best)->cost(), test->unassigned_cells.size);
    int total_cost = min_added_cuts + test->cost();
    if (total_cost < (*best)->cost()) {
        if (test->unassigned_cells.size == 0) {
            spdlog::info("found new best! ({} < {}) {}", test->cost(), (*best)->cost(), (void*)test);
            *best = test;
        }
    } else {
        spdlog::debug("PRUNING ({} > {})", total_cost, (*best)->cost());
        if (test->cost() < (*best)->cost()) {
            spdlog::debug("YOU PRUNED BASED ON GUARANTEED NET CUTS");
        }
        ret = true;
    }
    return ret;
}
