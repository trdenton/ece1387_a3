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
    pnode* parent;
    pnode* left;
    pnode* right;
};

class traverser {
    int best;
    pnode* root;
    queue<a3::partition*> q_bfs;
    vector<cell*> cells;
    public:
        void bfs_step();
        traverser(pnode*, vector<cell*>);
};

bool cell_sort_most_nets(cell* a, cell* b);
#endif
