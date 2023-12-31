#include "circuit.h"
#include "bitfield.h"
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <iterator>
#include "spdlog/spdlog.h"
#include "partition.h"
#include <thread>
#include <unistd.h>
#include <condition_variable>
#include <cassert>

using namespace std; 

/****
*
* circuit class functions
*
****/

circuit::circuit(string file) {
    string line;
    ifstream infile (file);
    spdlog::debug("Reading input file {}", file);

    if (infile.is_open()) {


        bool done = false;
        while (!done) {
            getline(infile, line);
            std::stringstream ss(line);
            istream_iterator<std::string> begin(ss);
            istream_iterator<std::string> end;
            vector<string> vstrings(begin, end);

            if (vstrings[0] == "-1") {
                done = true;
            } else {
                add_cell_connections(vstrings);
            }

        }
        infile.close();
    } else {
        spdlog::error("Could not open {}", file);
    }
}

net* circuit::get_net(int label) {
    auto it = std::find_if(nets.begin(),nets.end(),[label](net* n){ return n->label == label; });
    return *it;
}

void circuit::add_net(string s) {
    net* n = new net(s);
    if (std::find_if(nets.begin(),nets.end(),[n](net* ne){return ne->label == n->label;}) == nets.end()) {
        nets.push_back(n);
    } else {
        delete(n);
    }
}

void circuit::add_cell_connections(vector<string> toks) {
    cell* c = new cell(toks);
    cells.push_back(c);
    cellmap[c->label] = c;

    vector<string> s_nets = std::vector<string>(toks.begin()+1,toks.end()-1);
    for(string s_net : s_nets) {
        int net = stoi(s_net);
        add_net(s_net);
        c->add_net(s_net);
        get_net(net)->add_cell(*c);
    }

}

cell* circuit::get_cell(int label) {
    return cellmap[label];
}

circuit::~circuit() {
	for(auto n : nets) {
		delete n;
	}
	for(auto c : cells) {
		delete c;
	}
}

double circuit::get_display_width() {
    return PNODE_DIAMETER*(double)(2<<(get_n_cells()-1));
}

double circuit::get_display_height() {
    return PNODE_DIAMETER*(double)(get_n_cells());
}

/****
*
* cell class functions
*
****/

cell::cell(vector<string> s) {
    x = 0;
    y = 0;
    label = stoi(s[0]);
    //nets = std::vector<string>(s.begin()+1,s.end()-1);
}

// make a supercell
cell::cell(vector<cell*> cells) {
    x = 0;
    y = 0;
    label = 0;
    for(auto& c : cells) {
        for(auto& n: c->net_labels.to_vec()) {
            add_net(n);
        }
    }
}

void cell::add_net(int i) {
    net_labels.set(i); 
}

void cell::add_net(string s) {
    assert(s.length() > 0);
    int nl = stoi(s);
    add_net(nl);
}

int cell::get_num_nets() {
    return net_labels.size;
}

void cell::add_net(net& n) {
    add_net(n.label);
}

int cell::get_num_mutual_net_labels(cell* c) {
    return net_labels.intersection_with(c->net_labels).size;
}


/****
*
* net class functions
*
****/

void net::add_cell(string s) {
    assert(s.length() > 0);
    cell_labels.set(stoi(s));
}

void net::add_cell(int i) {
    cell_labels.set(i);
}


void net::add_cell(cell& c) {
    add_cell(c.label);
}

net::net(string l) {
    label = stoi(l);
}
