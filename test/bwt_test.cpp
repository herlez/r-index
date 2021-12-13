#include "bwt.hpp"
#include "util/io.hpp"

#include <gtest/gtest.h>

TEST(BWT, DefaultConstructor) {
}

TEST(BWT, EmptyInput) {
}

TEST(BWT, ContainsZero) {
  unsigned char test_char = '\00';
  std::string text;
  text.push_back(test_char);
  text.push_back(test_char);
  text.push_back(test_char);
  EXPECT_EQ(text.size(), 3);

  alx::bwt bwt(text);
  EXPECT_EQ(bwt.last_row[0], test_char);
  EXPECT_EQ(bwt.last_row[1], test_char);
  EXPECT_EQ(bwt.last_row[2], test_char);
  EXPECT_EQ(bwt.to_text(), text);
}

TEST(BWT, ContainsOne) {
  unsigned char test_char = '\01';
  std::string text;
  text.push_back(test_char);
  text.push_back(test_char);
  text.push_back(test_char);
  EXPECT_EQ(text.size(), 3);

  alx::bwt bwt(text);
  EXPECT_EQ(bwt.last_row[0], test_char);
  EXPECT_EQ(bwt.last_row[1], test_char);
  EXPECT_EQ(bwt.last_row[2], test_char);
  EXPECT_EQ(bwt.to_text(), text);
}

TEST(BWT, ContainsTwo) {
  unsigned char test_char = '\02';
  std::string text;
  text.push_back(test_char);
  text.push_back(test_char);
  text.push_back(test_char);
  EXPECT_EQ(text.size(), 3);

  alx::bwt bwt(text);
  EXPECT_EQ(bwt.last_row[0], test_char);
  EXPECT_EQ(bwt.last_row[1], test_char);
  EXPECT_EQ(bwt.last_row[2], test_char);
  EXPECT_EQ(bwt.to_text(), text);
}

TEST(BWT, FullAlphabet) {
  std::string text;
  for (size_t i = 0; i < 1024; ++i) {
    text.push_back(i % 256);
  }
  EXPECT_EQ(text.size(), 1024);

  alx::bwt bwt(text);
  EXPECT_EQ(bwt.to_text(), text);
}

TEST(BWT, ThreeBrothersRaw) {
    std::string text = alx::io::load_text("/home/herlez/text/the_three_brothers.txt");

    std::vector<int64_t> sa(text.size() + 100);
    std::vector<uint8_t> last_row;
    int64_t primary_index = libsais64_bwt(reinterpret_cast<const uint8_t*>(text.data()), last_row.data(), 
    sa.data(), text.size(), int64_t{100}, nullptr);
}

TEST(BWT, ThreeBrothers) {
    std::string text = alx::io::load_text("/home/herlez/text/the_three_brothers.txt");
    alx::bwt bwt(text);
}