////////////////////////////////////////////////////////////////////////////////
// Copyright Georgy Guminov 2023-2024.
// Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include <cstddef>
#include <dst/partial/dynamic_min_segment_tree.hpp>
#include <random>
#include <ranges>

#include "reference/min_seg_tree_reference.hpp"
#include "tools/generate_index_range.hpp"

using dst::DynamicMinSegmentTree;
using std::size_t;
using std::views::iota;
using UniformSizeTDistr = std::uniform_int_distribution<std::size_t>;
using GenerateIndRng = GenerateIndexRange<std::size_t>;

// NOLINTBEGIN(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)

TEST(DynamicMinSegmentTree, Construct) {
  [[maybe_unused]] auto tree = DynamicMinSegmentTree<int, int>(0, 42, 34);
}

TEST(DynamicMinSegmentTree, SimpleRangeGet) {
  auto tree = DynamicMinSegmentTree<int, int>(0, 42, 34);
  EXPECT_EQ(tree.rangeGet(5, 17), 34);
}

TEST(DynamicMinSegmentTree, RangeGetAfterUpdate) {
  auto tree = DynamicMinSegmentTree<int, int, std::plus<int>>(0, 42, 34);
  tree.update(12, 22, 4);
  EXPECT_EQ(tree.rangeGet(5, 17), 34);
  EXPECT_EQ(tree.rangeGet(12, 18), 34 + 4);
}

TEST(DynamicMinSegmentTree, RangeGetAfterSet) {
  auto tree = DynamicMinSegmentTree<int, int>(0, 42, 34);
  tree.set(12, 22, 4);
  EXPECT_EQ(tree.rangeGet(13, 20), 4);
  EXPECT_EQ(tree.rangeGet(2, 35), 4);
}

TEST(DynamicMinSegmentTree, UpdateAndSet) {
  auto tree = DynamicMinSegmentTree<int, int, std::plus<int>>(0, 42, 34);
  tree.update(12, 22, 4);
  tree.set(17, 27, 66);
  EXPECT_EQ(tree.rangeGet(5, 17), 34);
  EXPECT_EQ(tree.rangeGet(12, 18), 38);
}

TEST(DynamicMinSegmentTree, UpdateSetAndCopy) {
  auto tree = DynamicMinSegmentTree<int, int, std::plus<int>>(0, 42, 34);
  tree.update(12, 22, 4);
  tree.set(17, 27, 66);

  const auto copy = tree;

  EXPECT_EQ(copy.rangeGet(5, 17), 34);
  EXPECT_EQ(copy.rangeGet(12, 18), 34 + 4);

  EXPECT_EQ(tree.rangeGet(5, 17), 34);
  EXPECT_EQ(tree.rangeGet(12, 18), 34 + 4);
}

TEST(DynamicMinSegmentTree, UpdateSetAndCopyAssign) {
  auto tree = DynamicMinSegmentTree<int, int, std::plus<int>>(0, 42, 34);
  auto dest = DynamicMinSegmentTree<int, int, std::plus<int>>(0, 37, 34);
  tree.update(12, 22, 4);
  tree.set(17, 27, 66);

  dest = tree;

  EXPECT_EQ(dest.rangeGet(5, 17), 34);
  EXPECT_EQ(dest.rangeGet(12, 18), 34 + 4);

  EXPECT_EQ(tree.rangeGet(5, 17), 34);
  EXPECT_EQ(tree.rangeGet(12, 18), 34 + 4);
}

TEST(DynamicMinSegmentTree, UpdateSetAndMove) {
  auto tree = DynamicMinSegmentTree<int, int, std::plus<int>>(0, 42, 34);
  tree.update(12, 22, 4);
  tree.set(17, 27, 66);

  const auto moved = std::move(tree);

  EXPECT_EQ(moved.rangeGet(5, 17), 34);
  EXPECT_EQ(moved.rangeGet(12, 18), 34 + 4);
}

TEST(DynamicMinSegmentTree, LadderUpRight) {
  auto tree = DynamicMinSegmentTree<int, int>(0, 42, 0);
  tree.set(41, 42, 10000000);
  tree.set(40, 41, 1000000);
  tree.set(39, 40, 100000);
  tree.set(38, 39, 10000);
  tree.set(37, 38, 1000);
  tree.set(36, 37, 100);
  tree.set(35, 36, 10);
  tree.set(34, 35, 1);

  EXPECT_EQ(tree.rangeGet(0, 42), 0);
  EXPECT_EQ(tree.rangeGet(36, 42), 100);
  EXPECT_EQ(tree.rangeGet(0, 36), 0);
  EXPECT_EQ(tree.rangeGet(39, 42), 100000);
  EXPECT_EQ(tree.rangeGet(0, 39), 0);
}

TEST(DynamicMinSegmentTree, LadderUpLeft) {
  auto tree = DynamicMinSegmentTree<int, int>(0, 42, 0);
  tree.set(0, 1, 10000000);
  tree.set(1, 2, 1000000);
  tree.set(2, 3, 100000);
  tree.set(3, 4, 10000);
  tree.set(4, 5, 1000);
  tree.set(5, 6, 100);
  tree.set(6, 7, 10);
  tree.set(7, 7, 1);

  EXPECT_EQ(tree.rangeGet(0, 42), 0);
  EXPECT_EQ(tree.rangeGet(0, 6), 100);
  EXPECT_EQ(tree.rangeGet(6, 42), 0);
  EXPECT_EQ(tree.rangeGet(0, 3), 100000);
  EXPECT_EQ(tree.rangeGet(3, 42), 0);
}

TEST(DynamicMinSegmentTree, LadderDownRight) {
  auto tree = DynamicMinSegmentTree<int, int>(0, 42, 0);
  tree.set(41, 42, -10000000);
  tree.set(40, 41, -1000000);
  tree.set(39, 40, -100000);
  tree.set(38, 39, -10000);
  tree.set(37, 38, -1000);
  tree.set(36, 37, -100);
  tree.set(35, 36, -10);
  tree.set(34, 35, -1);

  EXPECT_EQ(tree.rangeGet(0, 42), -10000000);
  EXPECT_EQ(tree.rangeGet(36, 42), -10000000);
  EXPECT_EQ(tree.rangeGet(0, 36), -10);
  EXPECT_EQ(tree.rangeGet(39, 42), -10000000);
  EXPECT_EQ(tree.rangeGet(0, 39), -10000);
}

TEST(DynamicMinSegmentTree, LadderDownLeft) {
  auto tree = DynamicMinSegmentTree<int, int>(0, 42, 0);
  tree.set(0, 1, -10000000);
  tree.set(1, 2, -1000000);
  tree.set(2, 3, -100000);
  tree.set(3, 4, -10000);
  tree.set(4, 5, -1000);
  tree.set(5, 6, -100);
  tree.set(6, 7, -10);
  tree.set(7, 7, -1);

  EXPECT_EQ(tree.rangeGet(0, 42), -10000000);
  EXPECT_EQ(tree.rangeGet(0, 6), -10000000);
  EXPECT_EQ(tree.rangeGet(6, 42), -10);
  EXPECT_EQ(tree.rangeGet(0, 3), -10000000);
  EXPECT_EQ(tree.rangeGet(3, 42), -10000);
}

TEST(DynamicMinSegmentTree, FuzzTestSetUpdateGet) {
  constexpr auto treeEnd = size_t{1000};
  auto tree =
      DynamicMinSegmentTree<size_t, int, std::plus<int>>(0, treeEnd, 0);
  auto reference = MinSegTreeReference<size_t, int>(0, treeEnd, 0);

  std::mt19937 generator(42);

  for (size_t i : iota(0, 100)) {
    const auto [rngBegin, rngEnd] = GenerateIndRng(0, treeEnd)(generator);
    const int setVal = std::uniform_int_distribution(0, 1000)(generator);
    tree.set(rngBegin, rngEnd, setVal);
    reference.set(rngBegin, rngEnd, setVal);
  }

  for (size_t i : iota(0, 100)) {
    const auto [rngBegin, rngEnd] = GenerateIndRng(0, treeEnd)(generator);
    const auto updVal = std::uniform_int_distribution(0, 1000)(generator);
    tree.update(rngBegin, rngEnd, updVal);
    reference.update(rngBegin, rngEnd, std::plus<int>(), updVal);

    for (auto idx : iota(0, 1000)) {
      const auto treeRes = tree.get(idx);
      const auto refRes = reference.get(idx);
      EXPECT_EQ(treeRes, refRes);
    }
  }
}

TEST(DynamicMinSegmentTree, FuzzTestMixedSetUpdateGet) {
  constexpr auto treeEnd = std::size_t{1000};
  auto tree =
      DynamicMinSegmentTree<size_t, int, std::plus<int>>(0, treeEnd, 0);
  auto reference = MinSegTreeReference<size_t, int>(0, treeEnd, 0);

  std::mt19937 generator(54);

  for (size_t i : iota(0, 100)) {
    const auto [rngBegin, rngEnd] = GenerateIndRng(0, treeEnd)(generator);

    if (std::bernoulli_distribution()(generator)) {
      const auto setVal = std::uniform_int_distribution(0, 1000)(generator);
      tree.set(rngBegin, rngEnd, setVal);
      reference.set(rngBegin, rngEnd, setVal);
    } else {
      const auto updVal = std::uniform_int_distribution(0, 1000)(generator);
      tree.update(rngBegin, rngEnd, updVal);
      reference.update(rngBegin, rngEnd, std::plus<>(), updVal);
    }

    for (size_t idx : iota(size_t{0}, treeEnd)) {
      const auto treeRes = tree.get(idx);
      const auto refRes = reference.get(idx);
      EXPECT_EQ(treeRes, refRes);
    }
  }
}

TEST(DynamicMinSegmentTree, FuzzTestSetRangeGet) {
  constexpr auto treeEnd = size_t{1000};
  auto tree = DynamicMinSegmentTree<size_t, int>(0, treeEnd, 0);
  auto reference = MinSegTreeReference<size_t, int>(0, treeEnd, 0);

  std::mt19937 generator(42);

  for (size_t i : iota(0, 100)) {
    const auto [rngBegin, rngEnd] = GenerateIndRng(0, treeEnd)(generator);
    const auto setVal = std::uniform_int_distribution(0, 1000)(generator);
    tree.set(rngBegin, rngEnd, setVal);
    reference.set(rngBegin, rngEnd, setVal);
  }

  for (size_t i : iota(0, 50)) {
    const auto [rngBegin, rngEnd] = GenerateIndRng(0, treeEnd)(generator);
    const auto treeRes = tree.rangeGet(rngBegin, rngEnd);
    const auto refRes = reference.rangeGet(rngBegin, rngEnd);
    EXPECT_EQ(treeRes, refRes);
  }
}

TEST(DynamicMinSegmentTree, FuzzTestMixedSetUpdateRangeGet) {
  constexpr auto treeEnd = size_t{1000};
  auto tree =
      DynamicMinSegmentTree<size_t, int, std::plus<int>>(0, treeEnd, 0);
  auto reference = MinSegTreeReference<size_t, int>(0, treeEnd, 0);

  std::mt19937 generator(54);

  for (size_t i : iota(0, 100)) {
    const auto [rngBegin, rngEnd] = GenerateIndRng(0, treeEnd)(generator);
    if (std::bernoulli_distribution()(generator)) {
      const auto setVal = std::uniform_int_distribution(0, 1000)(generator);
      tree.set(rngBegin, rngEnd, setVal);
      reference.set(rngBegin, rngEnd, setVal);
    } else {
      const auto updVal = std::uniform_int_distribution(0, 1000)(generator);
      tree.update(rngBegin, rngEnd, updVal);
      reference.update(rngBegin, rngEnd, std::plus<int>(), updVal);
    }
  }

  for (size_t i : iota(0, 50)) {
    const auto [rngBegin, rngEnd] = GenerateIndRng(0, treeEnd)(generator);
    const auto treeRes = tree.rangeGet(rngBegin, rngEnd);
    const auto refRes = reference.rangeGet(rngBegin, rngEnd);
    EXPECT_EQ(treeRes, refRes);
  }
}

// NOLINTEND(cppcoreguidelines-*, cert-*, readability-magic-numbers,
// cert-err58-cpp)
