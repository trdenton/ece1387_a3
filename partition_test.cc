#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <utility>
#include <cstdlib>
#include <numeric>
#include <time.h>
#include <string>
#include "circuit.h"
#include "partition.h"

TEST(Partition, test_assign) {
    circuit* c = new circuit("../data/partition_test");
    a3::partition* p = new a3::partition(c);
    cell* c1 = c->get_cell(1);
    cell* c2 = c->get_cell(2);
    cell* c3 = c->get_cell(3);
    cell* c4 = c->get_cell(4);

    p->assign_left(c1);
    ASSERT_EQ(p->cost(), 0);

    p->assign_right(c2);
    ASSERT_EQ(p->cost(), 1); // 1 and 2 only share one net (a),  hence size of cut set is 1

    // put cell 3 in the same as 2, should not increase cost
    p->assign_right(c3);
    ASSERT_EQ(p->cost(), 1); 

    // put cell 4 in the opposite as 3 for incremental cost 1
    p->assign_left(c4);
    ASSERT_EQ(p->cost(), 2);
    

    delete p;
    delete c;
}

TEST(Partition, test_initial_solution) {

    circuit* c = new circuit("../data/cct1");
    a3::partition* p = new a3::partition(c);
    p->initial_solution();

    ASSERT_EQ(p->unassigned_cells.size, 0);
    ASSERT_EQ(p->vr_cells.size + p->vl_cells.size, c->get_n_cells());

    ASSERT_EQ(p->vr_cells.size, p->vl_cells.size);

    delete p;
    delete c;
}


bool never_prune(a3::partition* test, a3::partition** best) {
    return false;
}
/*
TEST(Tree, bfs) {
    circuit* c = new circuit("../data/partition_test");
    a3::partition* p = new a3::partition(c);
    a3::partition** best = &p;
    spdlog::set_level(spdlog::level::debug);

    cell* c1 = c->get_cells()[0];
    cell* c2 = c->get_cells()[1];
    cell* c3 = c->get_cells()[2];
    cell* c4 = c->get_cells()[3];

    p->initial_solution();
    
    traverser* t = new traverser(c, best, never_prune);
    t->prune_imbalance = false;

    pnode* pn = t->bfs_step();
    ASSERT_EQ(pn->level, 0);

    for (int i = 0; i < 2; ++i) {
        pn = t->bfs_step();
        ASSERT_EQ(pn->level, 1);
    }

    for (int i = 0; i < 4; ++i) {
        pn = t->bfs_step();
        ASSERT_EQ(pn->level, 2);
    }

    for (int i = 0; i < 8; ++i) {
        pn = t->bfs_step();
        ASSERT_EQ(pn->level, 3);
    }

    for (int i = 0; i < 16; ++i) {
        pn = t->bfs_step();
        ASSERT_EQ(pn->level, 4);
    }


    ASSERT_EQ(t->bfs_step(),nullptr);

    delete t;
    delete p;
    delete c;
}
*/
/*
TEST(Tree, bfs_prune_imbalance) {
    circuit* c = new circuit("../data/partition_test");
    a3::partition* p = new a3::partition(c);
    a3::partition** best = &p;
    spdlog::set_level(spdlog::level::debug);

    cell* c1 = c->get_cells()[0];
    cell* c2 = c->get_cells()[1];
    cell* c3 = c->get_cells()[2];
    cell* c4 = c->get_cells()[3];

    p->initial_solution();
    
    traverser* t = new traverser(c, best, never_prune);
    t->prune_imbalance = true;

    pnode* pn = t->bfs_step();
    ASSERT_EQ(pn->level, 0);

    for (int i = 0; i < 2; ++i) {
        pn = t->bfs_step();
        ASSERT_EQ(pn->level, 1);
    }

    for (int i = 0; i < 4; ++i) {
        pn = t->bfs_step();
        ASSERT_EQ(pn->level, 2);
    }

    for (int i = 0; i < 6; ++i) {
        pn = t->bfs_step();
        ASSERT_EQ(pn->level, 3);
    }

    for (int i = 0; i < 6; ++i) {
        pn = t->bfs_step();
        ASSERT_EQ(pn->level, 4);
    }



    ASSERT_EQ(t->bfs_step(),nullptr);

    delete t;
    delete p;
    delete c;
}
*/

TEST(bitfield, basic) {
    for(unsigned long long i = 0; i < 128; ++i) {
        bitfield b = bitfield();
        b.set(i);
        for(unsigned long long j = 0; j < 128; ++j) {
            if (i != j) {
                ASSERT_FALSE(b.get(j));
            } else {
                ASSERT_TRUE(b.get(j));
            }
        }
    }
}

TEST(bitfield, union_with) {
    bitfield a, b;
    a.set(31);
    a.set(32);
    a.set(33);
    a.set(34);
    b.set(33);
    b.set(34);
    b.set(35);
    b.set(36);

    ASSERT_EQ(b.union_with(a).size, 6);

    ASSERT_FALSE(b.union_with(a).get(30));
    ASSERT_TRUE(b.union_with(a).get(31));
    ASSERT_TRUE(b.union_with(a).get(32));
    ASSERT_TRUE(b.union_with(a).get(33));
    ASSERT_TRUE(b.union_with(a).get(34));
    ASSERT_TRUE(b.union_with(a).get(35));
    ASSERT_TRUE(b.union_with(a).get(36));
    ASSERT_FALSE(b.union_with(a).get(37));
}
