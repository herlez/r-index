#include <gtest/gtest.h>

#include <wavelet_tree/wavelet_tree.hpp>

//// WAVELET TREE ////

TEST(WaveletTreePasta, EmptyText) {
  //std::string text{};
  std::vector<uint8_t> text{};
  auto wt = pasta::make_wt<pasta::BitVector>(text.begin(), text.end(), 256);
}

TEST(WavelWaveletTreePastaetTree, RankTest) {
  std::string str{"banana"};
  std::vector<uint8_t> text(str.begin(), str.end());
  auto wt = pasta::make_wt<pasta::BitVector>(text.begin(), text.end(), 256);

  for (size_t i = 0; i < text.size(); ++i) {
    for (size_t c = 0; c < 256; ++c) {
      EXPECT_EQ(wt.rank(i, c), std::count(text.begin(), text.begin() + i, c)) << "i: " << i << " c:" << c << '\n';
    }
  }
}

TEST(WaveletTreePasta, FullAlphabetText) {
  std::vector<uint8_t> text(512);

  for (size_t i = 0; i < text.size(); ++i) {
    text[i] = i % 256;
  }

  auto wt = pasta::make_wt<pasta::BitVector>(text.begin(), text.end(), 256);

  std::vector histogram(256, 0);
  for (size_t i = 0; i < text.size(); ++i) {
    for (size_t c = 0; c < 256; ++c) {
      EXPECT_EQ(wt.rank(i, c), histogram[c]) << "i: " << i << " c:" << c << '\n';
    }
    ++histogram[text[i]];
  }
}

//// WAVELET MATRIX ////
TEST(WaveletMatrixPasta, EmptyText) {
  //std::string text{};
  std::vector<uint8_t> text{};
  auto wm = pasta::make_wm<pasta::BitVector>(text.begin(), text.end(), 256);
}

TEST(WaveletMatrixPasta, RankTest) {
  std::string str{"banana"};
  std::vector<uint8_t> text(str.begin(), str.end());
  auto wm = pasta::make_wm<pasta::BitVector>(text.begin(), text.end(), 256);

  for (size_t i = 0; i < text.size(); ++i) {
    for (size_t c = 0; c < 256; ++c) {
      EXPECT_EQ(wm.rank(i, c), std::count(text.begin(), text.begin() + i, c)) << "i: " << i << " c:" << c << '\n';
    }
  }
}

TEST(WaveletMatrixPasta, FullAlphabetText) {
  std::vector<uint8_t> text(512);

  for (size_t i = 0; i < text.size(); ++i) {
    text[i] = i % 256;
  }

  auto wm = pasta::make_wm<pasta::BitVector>(text.begin(), text.end(), 256);

  std::vector histogram(256, 0);
  for (size_t i = 0; i < text.size(); ++i) {
    for (size_t c = 0; c < 256; ++c) {
      EXPECT_EQ(wm.rank(i, c), histogram[c]) << "i: " << i << " c:" << c << '\n';
    }
    ++histogram[text[i]];
  }
}