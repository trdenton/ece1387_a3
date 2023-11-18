#include <gtest/gtest.h>
#include <vector>
#include <unordered_set>
#include <utility>
#include "circuit.h"

// Basic file read sanity checks
TEST(FileRead, cct1) {
    circuit* c = new circuit("../data/cct1");

    ASSERT_EQ(c->get_n_cells(),12);

    delete c;
}

