#include <gtest/gtest.h>
#include <tdc/uint/uint40.hpp>
#include <numeric>
#include "util/spacer.hpp"

TEST(UInt40Test, VectorPacking) {
    std::vector<tdc::uint40_t> vec;
    size_t vec_size = size_t{1} << 16;

    alx::benchutil::spacer spacer;
    vec.resize(vec_size);
    size_t mem_ds = spacer.get();
    EXPECT_LE(vec_size*5, mem_ds);
    EXPECT_GT(vec_size*5+100, mem_ds);
}
