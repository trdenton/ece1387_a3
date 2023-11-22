#ifndef __PARTITION_H__
#define __PARTITION_H__
#include "circuit.h"
#include <queue>
#include <vector>
#include <stack>

namespace a3 {
    struct partition {
        vector<cell*> vr;
        vector<cell*> vl;
        vector<cell*> unassigned;
        circuit* circ;

        partition(circuit*);
        int cost();
        bool assign(vector<cell*>&, cell*);
        bool assign_left(cell* c);
        bool assign_right(cell* c);
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

#endif
