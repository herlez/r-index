#include <gtest/gtest.h>

#include "r_index_alx.hpp"
#include "r_index_prezza.hpp"
//#include "r_index_subsampled.hpp"

size_t naive_occ(std::string const& text, std::string const& pattern) {
  if (pattern.size() == 0) {
    return 0;
  }
  int count = 0;
  size_t nPos = text.find(pattern, 0);  // first occurrence
  while (nPos != std::string::npos) {
    count++;
    nPos = text.find(pattern, nPos + 1);
  }
  return count;
}

template <typename t_index>
void test_default_constructor() {
  t_index index;
  EXPECT_EQ(index.occ("Example"), 0);
}

template <typename t_index>
void test_empty_text() {
  std::string text{""};
  t_index index(text);
  EXPECT_EQ(index.occ("Example"), 0);
}

template <typename t_index>
void test_alphabet_from_to(size_t min_char, size_t max_char) {
  std::string text{""};
  for (size_t i = 0; i < 512; ++i) {
    text.push_back(std::clamp(i % max_char, min_char, max_char));
  }

  t_index index(text);

  for (size_t c = min_char; c < 256; ++c) {
    std::string pattern{};
    pattern.push_back((unsigned char)c);
    ASSERT_EQ(index.occ(pattern), naive_occ(text, pattern)) << "c: " << c;
  }
}

template <typename t_index>
void test_all_a() {
  std::string text(5, 'a');

  t_index index(text);

  for (size_t i = 1; i <= text.size(); ++i) {
    std::string pattern(i, 'a');
    EXPECT_EQ(index.occ(pattern), naive_occ(text, pattern)) << "i: " << i;
  }
}
// My r-index
TEST(RIndex, DefaultConstructor) {
  test_default_constructor<alx::r_index<>>();
}

TEST(RIndex, Empty) {
  test_empty_text<alx::r_index<>>();
}

TEST(RIndex, HalfAlphabet) {
  test_alphabet_from_to<alx::r_index<>>(0, 127);
}

TEST(RIndex, HalfAlphabetWithoutReserved) {
  test_alphabet_from_to<alx::r_index<>>(2, 127);
}

TEST(RIndex, AllA) {
  test_all_a<alx::r_index<>>();
}


// Prezza's r-index
TEST(RIndexPrezza, DefaultConstructor) {
  test_default_constructor<alx::r_index_prezza>();
}

TEST(RIndexPrezza, Empty) {
  test_empty_text<alx::r_index_prezza>();
}

//TEST(RIndexPrezza, FullAlphabet) {
//  ASSERT_TRUE(false) << "rindex doesn't accept 0 and 1\n";
//  test_full_alphabet<alx::r_index_prezza>();
//}

TEST(RIndexPrezza, HalfAlphabetWithoutReserved) {
  test_alphabet_from_to<alx::r_index_prezza>(2, 127);
}

TEST(RIndexPrezza, AllA) {
  test_all_a<alx::r_index_prezza>();
}