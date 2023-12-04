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

struct bitfield {
    // assumption: we never have more than 1024 nets.  we can get this from  
    unsigned long long bits[16];
    int size;

    bool get(int net_num) {
        assert(net_num < 1024);
        unsigned long long chunk = net_num/64;
        unsigned long long rem = net_num%64;
        unsigned long long mask = (1ULL<<rem);
        if ((bits[chunk] & mask) > 0ULL) {
            return true;
        }
        return false;
    };
    void set(int net_num) {
        assert(net_num < 1024);
        unsigned long long chunk = net_num/64;
        unsigned long long rem = net_num%64;
        unsigned long long mask = (1ULL<<rem);
        if ((bits[chunk] & mask) == 0ULL) {
            ++size;
        }
        bits[chunk] |= mask;
    };

    bitfield union_with(bitfield& other) {
        bitfield result;
        for(int i = 0; i < 16; ++i) {
            result.bits[i] = bits[i] | other.bits[i];
        }
        // fix size
        result.size = 0;
        for (int i = 0; i < 1024; ++i) {
            if (get(i)) {
                result.size++;
            }
        }
        return result;
    };

    bitfield() {size=0; memset(bits, 0, sizeof(bits));};
    bitfield(bitfield *other) {
        memcpy(bits, other->bits, sizeof(bits));
        size = other->size;
    }
};

namespace a3 {
    struct partition {
    private:
        bool _unassign(vector<cell*>& v, cell* c);
        std::map<string, vector<int>> cell_uncut_nets;
        vector<int> circ_uncut_nets;
        bitfield leftnets;
        bitfield rightnets;

    public:
        vector<cell*> vr;
        vector<cell*> vl;
        vector<cell*> unassigned;
        circuit* circ;
        int cost();

        int min_number_anchored_nets_cut();
        int num_guaranteed_cut_nets();
        void cut_nets_from_adding_cell(vector<cell*>, cell* c);
        partition(circuit*);
        partition(a3::partition *other);
        int calculate_cut_set();
        bitfield cut_nets;
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
