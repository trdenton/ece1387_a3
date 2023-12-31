#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <getopt.h>
#include <vector>
#include <algorithm>
#include "spdlog/spdlog.h"
#include "version.h"
#include "easygl/graphics.h"
#include "ui.h"
#include "circuit.h"
#include "partition.h"
#include "bitfield.h"

using namespace std;

void print_usage() {
    cout << "Usage: ./a3 -[hvfdi] -f circuit_file" << endl;
    cout << "\t-h: this help message" <<endl;
    cout << "\t-v: print version info" <<endl;
    cout << "\t-f circuit_file: the circuit file (required)" <<endl;
    cout << "\t-d: turn on debug log level" <<endl;
    cout << "\t-i: enable interactive (gui) mode" <<endl;
    cout << "\t-b: use bfs mode (youd better have a lot of RAM for cct3/4.....)" <<endl;
}

void print_version() {
    spdlog::info("a3 - Troy Denton 2023");
    spdlog::info("Version {}.{}", VERSION_MAJOR, VERSION_MINOR);
    spdlog::info("Commit {}", GIT_COMMIT);
    spdlog::info("Built {}" , __TIMESTAMP__);
}

pnode* run(circuit* c, traverser* t) {
    static vector<int> seen;
    pnode* pn = t->bfs ? t->bfs_step() : t->dfs_step();
    if (pn != nullptr) {
        if (std::find(seen.begin(), seen.end(), pn->level) == seen.end()) {
            seen.push_back(pn->level); 
            spdlog::info("at level {}/{}.  Visited nodes: {}", seen.size(), c->get_n_cells(), t->visited_nodes);
        }
    }
    return pn;
}

int main(int n, char** args) {
    string file = "";

    bool interactive = false;
    bool bfs = false;

    for(;;)
    {
        switch(getopt(n, args, "vhf:dib"))
        {
            case 'f':
                file = optarg;
                continue;

            case 'd':
                spdlog::set_level(spdlog::level::debug);
                continue;

            case 'v':
                print_version();
                return 0;

            case 'b':
                bfs = true;
                continue;


            case 'i':
                interactive = true;
                continue;
            case '?':
            case 'h':
            default :
                print_usage();
                return 1;

            case -1:
                break;
        }
        break;
    }

    print_version();

    if (file == "") {
        spdlog::error("Error: must provide input file");
        print_usage();
        return 1;
    }

    circuit* circ = new circuit(file);

    a3::partition* init = new a3::partition(circ); 
    a3::partition** best = &init;
    spdlog::info("Building initial solution");
    init->initial_solution();
    spdlog::info("Initial solution cost: {}", (*best)->cost());
    spdlog::info("init {}", init->to_string());
    spdlog::info("MAX COST: {}", circ->get_n_nets());

    spdlog::info("Traversal mode: {}", bfs ? "BFS" : "Lowest Bound");
    
    traverser* trav = new traverser(circ, best, prune_basic_cost);
    trav->bfs = bfs;
    trav->prune_imbalance  = true;
    trav->prune_lb = true;
    trav->prune_symmetry = true;
    spdlog::info("Traversing decision tree");

    if (interactive) {
        spdlog::info("Entering interactive mode");
        ui_init(circ, trav, run);
    } else {
        while (run(circ,trav) != nullptr) {}
    }

    spdlog::info("Final solution cost: {} @ {}", (*best)->cut_nets.size, (void*)*best);
    spdlog::info("best {}", (*best)->to_string());
    unsigned long long int total_possible_nodes = (2<<(circ->get_n_cells()-1))-1;
    spdlog::info("Visited/possible nodes: {}/{}", trav->visited_nodes, total_possible_nodes);
    

    if (interactive) {
        ui_teardown();
    }

    spdlog::info("Exiting");
    // TODO fix this... it segfaults /double frees
    //delete trav;
    delete circ;
    //delete init;
    return 0;
}
