#include <gtest/gtest.h>

#include <queries/query_support.hpp>
#include <wx_pc.hpp>

/*** WAVELET TREE ***/
using wt_pc_8 = wx_pc<uint8_t, true>;

TEST(WaveletTree, EmptyText) {
  std::string text{};
  wavelet_structure wm = wt_pc_8::compute(text.data(), text.size(), 8);
}

TEST(WaveletTree, RankTest) {
  std::string text{"banana"};
  wavelet_structure wm = wt_pc_8::compute(text.data(), text.size(), 8);

  query_support qs(wm);
  for (size_t i = 0; i < text.size(); ++i) {
    for (size_t c = 0; c < 256; ++c) {
      EXPECT_EQ(qs.rank(c, i), std::count(text.begin(), text.begin() + i, c)) << "i: " << i << " c:" << c << '\n';
    }
  }
}

TEST(WaveletTree, FullAlphabetText) {
  std::string text(512, '\00');
  for (size_t i = 0; i < text.size(); ++i) {
    text[i] = i % 128;  // NO SUPPORTS FOR CHARACTER >= '\127'
  }

  wavelet_structure wm = wt_pc_8::compute(text.data(), text.size(), 8);

  query_support qs(wm);
  std::vector histogram(256, 0);
  for (size_t i = 0; i < text.size(); ++i) {
    for (size_t c = 0; c < 256; ++c) {
      EXPECT_EQ(qs.rank(c, i), histogram[c]) << "i: " << i << " c:" << c << '\n';
    }
    ++histogram[text[i]];
  }
}

/*** WAVELET MATRIX ***/

TEST(WaveletMatrix, EmptyText) {
  std::string text{};
  wavelet_structure wm = wt_pc_8::compute(text.data(), text.size(), 8);
}

TEST(WaveletMatrix, RankTest) {
  std::string text{"banana"};

  wavelet_structure wm = wt_pc_8::compute(text.data(), text.size(), 8);

  query_support qs(wm);
  for (size_t i = 0; i < text.size(); ++i) {
    for (size_t c = 0; c < 256; ++c) {
      EXPECT_EQ(qs.rank(c, i), std::count(text.begin(), text.begin() + i, c)) << "i: " << i << " c:" << c << '\n';
    }
  }
}

TEST(WaveletMatrix, FullAlphabetText) {
  std::string text(512, '\00');

  for (size_t i = 0; i < text.size(); ++i) {
    text[i] = i % 128;  // NO SUPPORTS FOR CHARACTER >= '\127'
  }

  wavelet_structure wm = wt_pc_8::compute(text.data(), text.size(), 8);

  query_support qs(wm);
  std::vector histogram(256, 0);
  for (size_t i = 0; i < text.size(); ++i) {
    for (size_t c = 0; c < 256; ++c) {
      EXPECT_EQ(qs.rank(c, i), histogram[c]) << "i: " << i << " c:" << c << '\n';
    }
    ++histogram[text[i]];
  }
}