#ifndef __PARTITION_H__
#define __PARTITION_H__
#include "circuit.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bundled/format.h"
#include <queue>
#include <map>
#include <vector>
#include <stack>

const double PNODE_DIAMETER = 10.0;

namespace a3 {
    struct partition {
    private:
        bool _unassign(vector<cell*>& v, cell* c);
        std::map<string, vector<string>> cell_uncut_nets;

    public:
        vector<cell*> vr;
        vector<cell*> vl;
        vector<cell*> unassigned;
        circuit* circ;
        int cost();

        void cut_nets_from_adding_cell(vector<cell*>, cell* c);
        partition(circuit*);
        int calculate_cut_set();
        vector<string> cut_nets;
        void print_cut_nets(void);
        bool assign(vector<cell*>&, cell*);
        bool assign_left(cell* c);
        bool assign_right(cell* c);
        bool unassign(cell* c);
        int lb();
        void initial_solution();
        void initial_solution_random();
        cell* make_right_supercell();
        cell* make_left_supercell();
        partition* copy();
        string to_string();
    };
}

struct pnode {
    a3::partition* p;
    double x;
    double y;
    int level;
    pnode* parent;
    pnode* left;
    pnode* right;
};

class traverser {
    pnode* root;
    circuit* circ;
    queue<pnode*> q_bfs;
    vector<cell*> cells;
    a3::partition** best;
    bool (*prune)(a3::partition* test, a3::partition** best);
    public:
        bool prune_imbalance;
        bool prune_symmetry;
        bool prune_lb;
        vector<pnode*> pnodes;
        long long unsigned int visited_nodes;
        traverser(circuit* c, a3::partition** best, bool (*prune_fn)(a3::partition* test, a3::partition** best));
        ~traverser();
        pnode* bfs_step();
};

bool cell_sort_most_nets(cell* a, cell* b);
void del_tree(pnode* root);
bool prune_basic_cost(a3::partition* test, a3::partition** best);

#endif
