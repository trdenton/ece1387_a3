#ifndef __CIRCUIT_H__
#define __CIRCUIT_H__
#include <vector>
#include <map>
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
        vector<string> mutable cell_labels;
    public:
        int label;

        net(string l);
        vector<string> get_cell_labels();
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
        bool fixed;
        int num_nets;
        vector<int> net_labels;

    public:
        double x;
        double y;
        string label;
        cell(vector<string> s);
        void connect(cell* other);
        vector<int> get_net_labels();
        int get_num_nets();
        void add_net(net& n);
        void add_net(int);
        void add_net(string s);
        bool is_connected_to(cell* other);
        vector<int> get_mutual_net_labels(cell* other);
        cell(vector<cell*> cells);
};

const double CELL_DIAMETER = 10.0;
class circuit {
    private:
        map<string, cell*> cellmap;
        vector<cell*> cells;
        vector<net*> nets;

    public:
        circuit(string s);
        ~circuit();
        int get_n_cells() { return cells.size();}
        int get_n_nets() { return nets.size();}

        cell* get_cell(string label);
        void add_cell_connections(vector<string> toks);
        net* get_net(int label);
        vector<net*> get_nets() {return nets;};
        void add_net(string s);
        vector<cell*> get_cells() {return cells;};
        double get_display_width();
        double get_display_height();
};
#endif
