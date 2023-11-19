#ifndef __CIRCUIT_H__
#define __CIRCUIT_H__
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <iostream>
#include <queue>
#include <sstream>
#include <inttypes.h>
#include <limits.h>
#include "spdlog/spdlog.h"

using namespace std;

class cell;
class fabric;

class net {
    private:
        unordered_set<string> mutable cell_labels;
    public:
        string label;

        net(string l);
        unordered_set<string> get_cell_labels();
        bool operator==(const net& other) const {
            return this->label == other.label;
        }
        void add_cell(cell& n);
        void add_cell(string c);
        int num_pins();
        double get_weight();
};

class cell {
    private:
        double x;
        double y;
        bool fixed;
        unordered_set<string> net_labels;

    public:
        string label;
        cell(vector<string> s);
        void connect(cell* other);
        unordered_set<string> get_net_labels();
        void add_net(net& n);
        void add_net(string s);
        bool is_connected_to(cell* other);
        vector<string> get_mutual_net_labels(cell* other);
        cell(vector<cell*> cells);
};

class circuit {
    private:
        vector<cell*> cells;
        unordered_map<string, net*> nets;

    public:
        circuit(string s);
        ~circuit();
        int get_n_cells() { return cells.size();}
        int get_n_nets() { return nets.size();}

        cell* get_cell(string label);
        void add_cell_connections(vector<string> toks);
        net* get_net(string label);
        unordered_map<string,net*> get_nets() {return nets;};
        void add_net(string s);
        void foreach_cell(void (*fn)(circuit* circ, cell* c));
        void foreach_net(void (*fn)(circuit* circ, net* n));
        vector<cell*> get_cells() {return cells;};
};
#endif
