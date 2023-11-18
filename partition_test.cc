#include <gtest/gtest.h>
#include <vector>
#include <unordered_set>
#include <utility>
#include "circuit.h"
#include "partition.h"

// Basic file read sanity checks
TEST(Partition, test_assign) {
    circuit* c = new circuit("../data/partition_test");
    a3::partition* p = new a3::partition(c);
    cell* c1 = c->get_cell("1");
    cell* c2 = c->get_cell("2");

    ASSERT_TRUE(p->assign_left(c1));
    ASSERT_FALSE(p->assign_left(c1)); // should only go in once
    ASSERT_FALSE(p->assign_right(c1)); // should only go in once

    ASSERT_TRUE(p->assign_right(c2));

    delete p;
    delete c;
}
