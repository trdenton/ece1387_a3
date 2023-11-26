#include <gtest/gtest.h>
#include <vector>
#include <unordered_set>
#include <utility>
#include <cstdlib>
#include <numeric>
#include <time.h>
#include <string>
#include "circuit.h"
#include "partition.h"

void test_circuit_against_random(string f);
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
    p->initial_solution();

    ASSERT_TRUE(p->unassigned.empty());
    ASSERT_EQ(p->vr.size() + p->vl.size(), c->get_n_cells());

    ASSERT_EQ(p->vr.size(), p->vl.size());

    delete p;
    delete c;
}

TEST(Partition, test_initial_solution_against_random) {
    test_circuit_against_random("../data/cct1");
    test_circuit_against_random("../data/cct2");
    test_circuit_against_random("../data/cct3");
    test_circuit_against_random("../data/cct4");
}

void test_circuit_against_random(string f){
    srand(time(NULL));
    circuit* c = new circuit(f);
    a3::partition* p = new a3::partition(c);
    a3::partition* p_blank = new a3::partition(c);
    p->initial_solution();

    spdlog::info("score from initial solution: {}", p->cost());

    vector<int> rand_scores(0);
    for(int i = 0; i < 10; ++i) {
        a3::partition* p_rand = p_blank->copy();
        p_rand->initial_solution_random();
        rand_scores.push_back(p_rand->cost());
        delete p_rand;
    }

    float rand_avg = (float)std::accumulate(rand_scores.begin(), rand_scores.end(), 0.0) / (float)rand_scores.size();
    spdlog::info("average score of {} random solutions: {}", rand_scores.size(), rand_avg);
    std::sort(rand_scores.begin(),rand_scores.end());
    spdlog::info("median score of {} random solutions: {}", rand_scores.size(), rand_scores[rand_scores.size()/2]);
    spdlog::info("lowest score of {} random solutions: {}", rand_scores.size(), rand_scores[0]);

    delete p_blank;
    delete p;
    delete c;
}


bool never_prune(a3::partition* test, a3::partition*& best) {
    return false;
}

TEST(Tree, bfs) {
    circuit* c = new circuit("../data/partition_test");
    a3::partition* p = new a3::partition(c);
    spdlog::set_level(spdlog::level::debug);

    cell* c1 = c->get_cells()[0];
    cell* c2 = c->get_cells()[1];
    cell* c3 = c->get_cells()[2];
    cell* c4 = c->get_cells()[3];

    p->initial_solution();
    
    traverser* t = new traverser(c, p, never_prune);
    t->prune_imbalance = false;

    pnode* pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c2);

    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c3);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c3);

    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c4);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c4);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c4);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c4);

    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);

    ASSERT_EQ(t->bfs_step(),nullptr);

    delete t;
    delete p;
    delete c;
}

TEST(Tree, bfs_prune_imbalance) {
    circuit* c = new circuit("../data/partition_test");
    a3::partition* p = new a3::partition(c);
    spdlog::set_level(spdlog::level::debug);

    cell* c1 = c->get_cells()[0];
    cell* c2 = c->get_cells()[1];
    cell* c3 = c->get_cells()[2];
    cell* c4 = c->get_cells()[3];

    p->initial_solution();
    
    traverser* t = new traverser(c, p, never_prune);
    t->prune_imbalance = true;

    pnode* pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c2);

    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c3);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c3);

    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c4);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c4);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c4);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c4);

    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);
    pn = t->bfs_step();
    ASSERT_EQ(*pn->cell, c1);

    ASSERT_EQ(t->bfs_step(),nullptr);

    delete t;
    delete p;
    delete c;
}
TEST(Partition, unassign_cell) {

    circuit* c = new circuit("../data/cct1");
    a3::partition* p = new a3::partition(c);

    // this is normally done by initial_solution
    std::sort(p->unassigned.begin(), p->unassigned.end(), cell_sort_most_nets);

    ASSERT_TRUE(std::is_sorted(p->unassigned.begin(), p->unassigned.end(), cell_sort_most_nets));

    ASSERT_TRUE(p->assign_left(*c->get_cells().begin()));

    ASSERT_EQ(p->vr.size(), 0);
    ASSERT_EQ(p->vl.size(), 1);
    ASSERT_EQ(p->unassigned.size(), c->get_n_cells()-1);

    ASSERT_TRUE(p->unassign(*c->get_cells().begin()));

    ASSERT_EQ(p->vr.size(), 0);
    ASSERT_EQ(p->vl.size(), 0);
    ASSERT_EQ(p->unassigned.size(), c->get_n_cells());
    
    ASSERT_TRUE(std::is_sorted(p->unassigned.begin(), p->unassigned.end(), cell_sort_most_nets));

    delete p;
    delete c;
}
