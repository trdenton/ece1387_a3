#include <gtest/gtest.h>
#include <vector>
#include <unordered_set>
#include <utility>
#include <cstdlib>
#include "circuit.h"
#include "partition.h"

TEST(Partition, test_assign) {
    circuit* c = new circuit("../data/partition_test");
    a3::partition* p = new a3::partition(c);
    cell* c1 = c->get_cell("1");
    cell* c2 = c->get_cell("2");
    cell* c3 = c->get_cell("3");
    cell* c4 = c->get_cell("4");

    ASSERT_TRUE(p->assign_left(c1));
    ASSERT_FALSE(p->assign_left(c1)); // should only go in once
    ASSERT_FALSE(p->assign_right(c1)); // should only go in once
    ASSERT_EQ(p->cost(), 0);

    ASSERT_TRUE(p->assign_right(c2));
    ASSERT_EQ(p->cost(), 1); // 1 and 2 only share one net (a),  hence size of cut set is 1

    // put cell 3 in the same as 2, should not increase cost
    ASSERT_TRUE(p->assign_right(c3));
    ASSERT_EQ(p->cost(), 1); 

    // put cell 4 in the opposite as 3 for incremental cost 1
    ASSERT_TRUE(p->assign_left(c4));
    ASSERT_EQ(p->cost(), 2);
    

    delete p;
    delete c;
}

TEST(Partition, test_initial_solution) {

    circuit* c = new circuit("../data/cct1");
    a3::partition* p = new a3::partition(c);
    a3::partition* p_init = p->initial_solution();

    ASSERT_TRUE(p_init->unassigned.empty());
    ASSERT_EQ(p_init->vr.size() + p_init->vl.size(), c->get_n_cells());

    ASSERT_EQ(p_init->vr.size(), p_init->vl.size());

    delete p_init;
    delete p;
    delete c;
}
