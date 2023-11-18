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
    cell* c1 = c->get_cell("1");
    cell* c2 = c->get_cell("2");
    cell* c3 = c->get_cell("3");
    cell* c4 = c->get_cell("4");
    cell* c5 = c->get_cell("5");
    cell* c9 = c->get_cell("9");
    cell* c11 = c->get_cell("11");
    cell* c12 = c->get_cell("12");

    // these all share net 26
    ASSERT_TRUE( c1->is_connected_to(c3) );
    ASSERT_TRUE( c1->is_connected_to(c4) );
    ASSERT_TRUE( c1->is_connected_to(c5) );
    ASSERT_TRUE( c1->is_connected_to(c9) );
    ASSERT_TRUE( c1->is_connected_to(c11) );

    ASSERT_TRUE( c3->is_connected_to(c1) );
    ASSERT_TRUE( c3->is_connected_to(c4) );
    ASSERT_TRUE( c3->is_connected_to(c5) );
    ASSERT_TRUE( c3->is_connected_to(c9) );
    ASSERT_TRUE( c3->is_connected_to(c11) );

    ASSERT_TRUE( c4->is_connected_to(c1) );
    ASSERT_TRUE( c4->is_connected_to(c3) );
    ASSERT_TRUE( c4->is_connected_to(c5) );
    ASSERT_TRUE( c4->is_connected_to(c9) );
    ASSERT_TRUE( c4->is_connected_to(c11) );

    ASSERT_TRUE( c5->is_connected_to(c1) );
    ASSERT_TRUE( c5->is_connected_to(c3) );
    ASSERT_TRUE( c5->is_connected_to(c4) );
    ASSERT_TRUE( c5->is_connected_to(c9) );
    ASSERT_TRUE( c5->is_connected_to(c11) );

    ASSERT_TRUE( c9->is_connected_to(c1) );
    ASSERT_TRUE( c9->is_connected_to(c3) );
    ASSERT_TRUE( c9->is_connected_to(c4) );
    ASSERT_TRUE( c9->is_connected_to(c5) );
    ASSERT_TRUE( c9->is_connected_to(c11) );

    ASSERT_TRUE( c11->is_connected_to(c1) );
    ASSERT_TRUE( c11->is_connected_to(c3) );
    ASSERT_TRUE( c11->is_connected_to(c4) );
    ASSERT_TRUE( c11->is_connected_to(c5) );
    ASSERT_TRUE( c11->is_connected_to(c9) );

    // cell 2 is not on net 26
    ASSERT_FALSE( c2->is_connected_to(c12) );
    delete c;
}
