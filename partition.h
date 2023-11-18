#ifndef __PARTITION_H__
#define __PARTITION_H__
#include "circuit.h"
#include <queue>
#include <vector>
#include <stack>

/*
struct pnode {
    cell* cell;
    pnode* parent;
    pnode* left;
    pnode* right;
};
*/
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
        partition* initial_solution();
    };
}

/*
class traverser {
    queue<pnode*> q_bfs;
    public:
        bfs(pnode* root);
}
*/
#endif
