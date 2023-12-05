#include <gtest/gtest.h>
#include <vector>
#include <unordered_set>
#include <utility>
#include "circuit.h"

// Basic file read sanity checks
TEST(FileRead, cct1_count_cells) {
    circuit* c = new circuit("../data/cct1");
    ASSERT_EQ(c->get_n_cells(),12);
    delete c;
}

// Basic file read sanity checks
TEST(FileRead, cct1_count_nets) {
    circuit* c = new circuit("../data/cct1");
    ASSERT_EQ(c->get_n_nets(),40);
    delete c;
}

TEST(FileRead, cct1_conns) {
    circuit* c = new circuit("../data/cct1");
    cell* c2 = c->get_cell(2);
    net* n36 = c->get_net(36);

    // cell 2 should have the following nets:
    // 21 34 3 37 11

    for(int i = 0; i < 128; i++) {
        bool has_net = (i==21)||(i==34)||(i==3)||(i==3)||(i==37)||(i==11);
        ASSERT_EQ(c2->net_labels.get(i),has_net);
    }

    // net 6 should have the following cells:
    // 12 6

    for(int i = 0; i < 128; i++) {
        bool has_cell = (i==12)||(i==6);
        ASSERT_EQ(n36->cell_labels.get(i),has_cell);
    }

    delete c;
}
