#include "circuit.h"
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

net* circuit::get_net(string label) {
    //net n(label);
    //auto it = nets.find(n);
    //return &n;
    return nets[label];
}

void circuit::add_net(string s) {
    net* n = new net(s);
    if (nets.find(s) == nets.end()) {
        nets[s] = n;
    } else {
        delete(n);
    }
}

void circuit::add_cell_connections(vector<string> toks) {
    cell* c = new cell(toks);
    cells.push_back(c);

    vector<string> s_nets = std::vector<string>(toks.begin()+1,toks.end()-1);
    for(string net : s_nets) {
        add_net(net);
        c->add_net(net);
        nets[net]->add_cell(*c);
    }

}

cell* circuit::get_cell(string label) {
    for (auto b : cells) {
        if (b->label == label) {
            return b;
        }
    }
    return nullptr;
}

circuit::~circuit() {
}

void circuit::foreach_cell(void (*fn)(circuit* circ, cell* c)) {
    for(auto* c : cells) {
        fn(this,c);
    }
}

void circuit::foreach_net(void (*fn)(circuit* circ, net* c)) {
    for(auto& c : nets) {
        fn(this, c.second);
    }
}

/****
*
* cell class functions
*
****/

cell::cell(vector<string> s) {
    x = 0;
    y = 0;
    label = s[0];
    //nets = std::vector<string>(s.begin()+1,s.end()-1);
}

// make a supercell
cell::cell(vector<cell*> cells) {
    x = 0;
    y = 0;
    label = "[sc]";
    for(auto& c : cells) {
        for(auto& n: c->get_net_labels()) {
            add_net(n);
        }
    }
}

unordered_set<string> cell::get_net_labels() {
    return net_labels;
}

void cell::add_net(string s) {
    assert(s.length() > 0);
    net_labels.insert(s); 
}

void cell::add_net(net& n) {
    add_net(n.label);
}

bool cell::is_connected_to(cell* other) {
    return (get_mutual_net_labels(other).size() > 0);
}

vector<string> cell::get_mutual_net_labels(cell* other) {
    vector<string> my_net_labels_ordered;
    vector<string> other_net_labels_ordered;
    
    
    for (auto n : net_labels) {
        my_net_labels_ordered.push_back(n);
    }
    
    for (auto n : other->net_labels) {
        other_net_labels_ordered.push_back(n);
    }

    sort(my_net_labels_ordered.begin(), my_net_labels_ordered.end());
    sort(other_net_labels_ordered.begin(), other_net_labels_ordered.end());

    vector<string> intersection;

    set_intersection(my_net_labels_ordered.begin(), my_net_labels_ordered.end(),
                     other_net_labels_ordered.begin(), other_net_labels_ordered.end(),
                     back_inserter(intersection));
    return intersection;
}


/****
*
* net class functions
*
****/

void net::add_cell(string s) {
    assert(s.length() > 0);
    cell_labels.insert(s);
}

void net::add_cell(cell& c) {
    add_cell(c.label);
}

net::net(string l) {
    label = l;
}

unordered_set<string> net::get_cell_labels() {
    return cell_labels;
}

int net::num_pins() {
    return cell_labels.size();
}

double net::get_weight() {
    return (double)(2./(double)(num_pins()));
}

