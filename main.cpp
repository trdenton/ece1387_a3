#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <getopt.h>
#include "spdlog/spdlog.h"
#include "version.h"
#include "easygl/graphics.h"
#include "ui.h"
#include "circuit.h"
#include "partition.h"

using namespace std;

void print_usage() {
    cout << "Usage: ./a3 -[hvfdi] -f circuit_file" << endl;
    cout << "\t-h: this help message" <<endl;
    cout << "\t-v: print version info" <<endl;
    cout << "\t-f circuit_file: the circuit file (required)" <<endl;
    cout << "\t-d: turn on debug log level" <<endl;
    cout << "\t-i: enable interactive (gui) mode" <<endl;
}

void print_version() {
    spdlog::info("a3 - Troy Denton 2023");
    spdlog::info("Version {}.{}", VERSION_MAJOR, VERSION_MINOR);
    spdlog::info("Commit {}", GIT_COMMIT);
    spdlog::info("Built {}" , __TIMESTAMP__);
}

int main(int n, char** args) {
    string file = "";

    bool interactive = false;

    for(;;)
    {
        switch(getopt(n, args, "vhf:di"))
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

    a3::partition* best = new a3::partition(circ); 
    best->initial_solution();
    
    traverser* trav = new traverser(circ, best, prune_basic_cost);

    while (trav->bfs_step() != nullptr) {
    }

    

    if (interactive) {
        spdlog::info("Entering interactive mode");
        ui_init(circ);
        ui_teardown();
    }

    spdlog::info("Exiting");
    delete trav;
    delete circ;
    return 0;
}
