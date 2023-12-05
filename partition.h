#ifndef __PARTITION_H__
#define __PARTITION_H__
#include "circuit.h"
#include "bitfield.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bundled/format.h"
#include <queue>
#include <map>
#include <vector>
#include <stack>
#include <algorithm>


const double PNODE_DIAMETER = 10.0;

class cell;
class circuit;

namespace a3 {
    struct partition {

    public:
        circuit* circ;
        bitfield vr_cells;
        bitfield vl_cells;
        bitfield vr_nets;
        bitfield vl_nets;
        bitfield unassigned_cells;
        bitfield uncut_nets;
        bitfield cut_nets;

        partition();
        partition(a3::partition*);
        bitfield min_number_anchored_nets_cut();
        bitfield num_guaranteed_cut_nets();
        bitfield one_partition_full_cut_nets();

        cell* next_unassigned(std::vector<cell*>);
        int cost();
        void update_cut_nets();
        partition(circuit*);
        int calculate_cut_set();
        void print_cut_nets(void);
        void assign_left(cell* c);
        void assign_right(cell* c);
        int lb();
        void initial_solution();
        void initial_solution_random();
        std::string to_string();
    };
}

struct pnode {
    a3::partition p;
    double x;
    double y;
    int level;
    pnode* parent;
    pnode* left;
    pnode* right;
    pnode();
};

class traverser {
    pnode* root;
    circuit* circ;
    std::queue<pnode*> q_bfs;
    std::vector<cell*> cells;
    a3::partition** best;
    bool (*prune)(a3::partition* test, a3::partition** best);
    public:
        std::vector<cell*>::iterator cur_cell;
        bool prune_imbalance;
        bool prune_symmetry;
        bool prune_lb;
        std::vector<pnode*> pnodes;
        long long unsigned int visited_nodes;
        traverser(circuit* c, a3::partition** best, bool (*prune_fn)(a3::partition* test, a3::partition** best));
        ~traverser();
        pnode* bfs_step();
};

bool cell_sort_most_nets(cell* a, cell* b);
void del_tree(pnode* root);
bool prune_basic_cost(a3::partition* test, a3::partition** best);

#endif
