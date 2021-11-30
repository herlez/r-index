#include <gtest/gtest.h>

#include "r-index.hpp"

size_t naive_occ(std::string const& text, std::string const& pattern) {
  if(pattern.size() == 0) {return 0;}
  int count = 0;
  size_t nPos = text.find(pattern, 0);  // first occurrence
  while (nPos != std::string::npos) {
    count++;
    nPos = text.find(pattern, nPos + 1);
  }
  return count;
}

TEST(RIndex, DefaultConstructor) {
  alx::r_index index;
  EXPECT_EQ(index.occ("Example"), 0);
}

TEST(RIndex, Empty) {
  std::string text{""};
  alx::r_index index(text);
  EXPECT_EQ(index.occ("Example"), 0);
}

TEST(RIndex, FullAlphabet) {
  std::string text;
  for (size_t i = 0; i < 512; ++i) {
    text.push_back(i % 128);
  }

  alx::r_index index(text);

  for (size_t c = 0; c < 128; ++c) {
    std::string pattern{};
    pattern.push_back((unsigned char)c);
    EXPECT_EQ(index.occ(pattern), naive_occ(text, pattern));
  }
  for (size_t c = 128; c < 256; ++c) {
    std::string pattern{};
    EXPECT_EQ(index.occ(pattern), naive_occ(text, pattern));
  }
}

TEST(RIndex, AllA) {
  std::string text(5, 'a');

  alx::r_index index(text);

  for (size_t i = 0; i <= text.size(); ++i) {
    std::string pattern(i, 'a');
    EXPECT_EQ(index.occ(pattern, false), naive_occ(text, pattern)) << "i: " << i;
  }
}
