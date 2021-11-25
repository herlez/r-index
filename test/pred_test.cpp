#include <gtest/gtest.h>

#include <tdc/pred/index.hpp>

TEST(Predecessor, DefaultConstructor) {
  tdc::pred::Index pred;
  EXPECT_EQ(pred.predecessor(nullptr, 0, 0).exists, false);
}

TEST(Predecessor, EmptyVector) {
  std::vector<size_t> vec{};
  tdc::pred::Index pred(vec.data(), vec.size(), 7);
  EXPECT_EQ(pred.predecessor(vec.data(), 0, 0).exists, false);
}

TEST(Predecessor, OneElementVector) {
  {
    std::vector<size_t> vec{0};
    tdc::pred::Index pred(vec.data(), vec.size(), 7);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), 0).exists, true);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), 0).pos, 0);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), 1'000'000).exists, true);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), 1'000'000).pos, 0);
  }
  {
    std::vector<size_t> vec{10};
    tdc::pred::Index pred(vec.data(), vec.size(), 7);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), 0).exists, false);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), 1'000'000).exists, true);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), 1'000'000).pos, 0);
  }
  {
    std::vector<size_t> vec{std::numeric_limits<size_t>::max()};
    tdc::pred::Index pred(vec.data(), vec.size(), 7);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), 0).exists, false);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), 1'000'000).exists, false);
  }
}

TEST(Predecessor, BasicQueries) {
  std::vector<size_t> vec{10, 100, 1000};
  tdc::pred::Index pred(vec.data(), vec.size(), 7);

  for (size_t i = 0; i < vec[0]; ++i) {
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).exists, false);
  }

  for (size_t i = vec[0]; i < vec[1]; ++i) {
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).exists, true);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).pos, 0);
  }

  for (size_t i = vec[1]; i < vec[2]; ++i) {
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).exists, true);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).pos, 1);
  }
  for (size_t i = vec[2]; i < vec[2] + 100; ++i) {
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).exists, true);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).pos, 2);
  }
}

TEST(Predecessor, CopyConstructorQuery) {
  tdc::pred::Index pred;
  std::vector<size_t> vec{10, 100, 1000};
  {
    tdc::pred::Index pred_original(vec.data(), vec.size(), 7);
    pred = pred_original;
  }

  for (size_t i = 0; i < vec[0]; ++i) {
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).exists, false);
  }

  for (size_t i = vec[0]; i < vec[1]; ++i) {
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).exists, true);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).pos, 0);
  }

  for (size_t i = vec[1]; i < vec[2]; ++i) {
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).exists, true);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).pos, 1);
  }
  for (size_t i = vec[2]; i < vec[2] + 100; ++i) {
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).exists, true);
    EXPECT_EQ(pred.predecessor(vec.data(), vec.size(), i).pos, 2);
  }
}