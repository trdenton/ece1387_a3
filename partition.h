#ifndef __PARTITION_H__
#define __PARTITION_H__
#include "circuit.h"
#include <queue>
#include <vector>
#include <stack>

namespace a3 {
    struct partition {
    private:
        bool _unassign(vector<cell*>& v, cell* c);

    public:
        vector<cell*> vr;
        vector<cell*> vl;
        vector<cell*> unassigned;
        circuit* circ;

        partition(circuit*);
        int cost();
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
    };
}

struct pnode {
    cell* cell;
    a3::partition* p;
    pnode* parent;
    pnode* left;
    pnode* right;
};

class traverser {
    pnode* root;
    queue<pnode*> q_bfs;
    vector<cell*> cells;
    a3::partition* best;
    bool (*prune)(a3::partition* test, a3::partition*& best);
    public:
        long long unsigned int visited_nodes;
        traverser(circuit* c, a3::partition* best, bool (*prune_fn)(a3::partition* test, a3::partition*& best));
        ~traverser();
        pnode* bfs_step();
};

bool cell_sort_most_nets(cell* a, cell* b);
pnode* build_tree(circuit* c);
void del_tree(pnode* root);
bool prune_basic_cost(a3::partition* test, a3::partition*& best);
#endif
