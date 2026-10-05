#include <gtest/gtest.h>
#include "aican/parser.hpp"

TEST(Parser, CandumpBasic) {
    std::string txt =
        "(1700000000.000000) can0 0x100#DEADBEEF\n"
        "(1700000000.005000) can0 0x200#1122\n";
    auto f = aican::parse_candump(txt);
    ASSERT_EQ(f.size(), 2u);
    EXPECT_EQ(f[0].can_id, 0x100u);
    EXPECT_EQ(f[0].data.size(), 4u);
    EXPECT_EQ(f[0].data[0], 0xDEu);
}

TEST(Parser, CsvBasic) {
    std::string txt = "timestamp,id,data\n0.0,0x100,AABB\n";
    auto f = aican::parse_csv(txt);
    ASSERT_EQ(f.size(), 1u);
    EXPECT_EQ(f[0].can_id, 0x100u);
}
