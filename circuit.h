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
#include "partition.h"
#include "bitfield.h"

using namespace std;

class cell;

class net {
    public:
        bitfield cell_labels;
        int label;

        net(string l);
        bool operator==(const net& other) const {
            return this->label == other.label;
        }
        void add_cell(int i);
        void add_cell(cell& n);
        void add_cell(string c);
};

class cell {
    public:
        bitfield net_labels;
        double x;
        double y;
        int label;
        cell(vector<string> s);
        void connect(cell* other);
        int get_num_nets();
        void add_net(net& n);
        void add_net(int);
        void add_net(string s);
        cell(vector<cell*> cells);
        int get_num_mutual_net_labels(cell* c);
        bool operator==(const cell& other) const {
            return this->label == other.label;
        }
};

const double CELL_DIAMETER = 10.0;
class circuit {
    private:
        map<int, cell*> cellmap;
        vector<cell*> cells;
        vector<net*> nets;

    public:
        circuit(string s);
        ~circuit();
        int get_n_cells() { return cells.size();}
        int get_n_nets() { return nets.size();}

        cell* get_cell(int label);
        void add_cell_connections(vector<string> toks);
        net* get_net(int label);
        vector<net*> get_nets() {return nets;};
        void add_net(string s);
        vector<cell*> get_cells() {return cells;};
        double get_display_width();
        double get_display_height();
};
#endif
