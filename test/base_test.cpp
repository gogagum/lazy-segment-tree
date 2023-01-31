#include <gtest/gtest.h>
#include <lazy_segment_tree.hpp>

using lst::LazySegmentTree;

TEST(LazySegmentTree, Construct) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
}

TEST(LazySegmentTree, QueryMiddle) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    EXPECT_EQ(tree.get(21), 54);
}

TEST(LazySegmentTree, QueryEnd) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    EXPECT_EQ(tree.get(42), 0);
}

TEST(LazySegmentTree, QueryAfterEnd) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    EXPECT_EQ(tree.get(73), 0);
}

TEST(LazySegmentTree, QueryBegin) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    EXPECT_EQ(tree.get(0), 54);
}

TEST(LazySegmentTree, QueryBeforeBegin) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    EXPECT_EQ(tree.get(-5), 0);
}

TEST(LazySegmentTree, OneUpdate) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    tree.update(15, 37, 18);
    EXPECT_EQ(tree.get(5), 54);
    EXPECT_EQ(tree.get(14), 54);
    EXPECT_EQ(tree.get(15), 54 + 18);
    EXPECT_EQ(tree.get(27), 54 + 18);
    EXPECT_EQ(tree.get(36), 54 + 18);
    EXPECT_EQ(tree.get(37), 54);
}

TEST(LazySegmentTree, TwoSameUpdates) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    tree.update(15, 37, 18);
    tree.update(15, 37, 14);
    EXPECT_EQ(tree.get(5), 54);
    EXPECT_EQ(tree.get(14), 54);
    EXPECT_EQ(tree.get(15), 54 + 18 + 14);
    EXPECT_EQ(tree.get(27), 54 + 18 + 14);
    EXPECT_EQ(tree.get(36), 54 + 18 + 14);
    EXPECT_EQ(tree.get(37), 54);
}

TEST(LazySegmentTree, TwoIntersectingUpdates) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    tree.update(15, 37, 18);
    tree.update(20, 41, 14);
    EXPECT_EQ(tree.get(5), 54);
    EXPECT_EQ(tree.get(14), 54);
    EXPECT_EQ(tree.get(15), 54 + 18);
    EXPECT_EQ(tree.get(17), 54 + 18);
    EXPECT_EQ(tree.get(19), 54 + 18);
    EXPECT_EQ(tree.get(20), 54 + 18 + 14);
    EXPECT_EQ(tree.get(25), 54 + 18 + 14);
    EXPECT_EQ(tree.get(35), 54 + 18 + 14);
    EXPECT_EQ(tree.get(36), 54 + 18 + 14);
    EXPECT_EQ(tree.get(37), 54 + 14);
    EXPECT_EQ(tree.get(38), 54 + 14);
    EXPECT_EQ(tree.get(39), 54 + 14);
    EXPECT_EQ(tree.get(40), 54 + 14);
    EXPECT_EQ(tree.get(41), 54);
    EXPECT_EQ(tree.get(42), 0);
}

TEST(LazySegmentTree, TwoNonIntersectingUpdates) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    tree.update(5, 8, 18);
    tree.update(17, 38, 14);
    EXPECT_EQ(tree.get(3), 54);
    EXPECT_EQ(tree.get(5), 54 + 18);
    EXPECT_EQ(tree.get(6), 54 + 18);
    EXPECT_EQ(tree.get(8), 54);
    EXPECT_EQ(tree.get(12), 54);
    EXPECT_EQ(tree.get(16), 54);
    EXPECT_EQ(tree.get(17), 54 + 14);
    EXPECT_EQ(tree.get(23), 54 + 14);
    EXPECT_EQ(tree.get(38), 54);
    EXPECT_EQ(tree.get(40), 54);
    EXPECT_EQ(tree.get(42), 0);
}

TEST(LazySegmentTree, SimpleRangeGetAll) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    auto rangeGetResult = tree.rangeGet(0, 42);
    EXPECT_EQ(rangeGetResult, 54 * 42);
}

TEST(LazySegmentTree, SimpleRangeGetPart) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    auto rangeGetResult = tree.rangeGet(12, 23);
    EXPECT_EQ(rangeGetResult, 54 * (23 - 12));
}

TEST(LazySegmentTree, SimpleRangeGetMoreThanAll) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    auto rangeGetResult = tree.rangeGet(-5, 50);
    EXPECT_EQ(rangeGetResult, 54 * 42);
}

TEST(LazySegmentTree, RangeGetAfterUpdate) {
    auto tree = LazySegmentTree<int, int>(0, 42, 54);
    tree.update(15, 37, 18);

    auto getInsideUpdateRes = tree.rangeGet(17, 30);
    EXPECT_EQ(getInsideUpdateRes, (54 + 18) * (30 - 17));

    auto getOfUpdateSizeRes = tree.rangeGet(15, 37);
    EXPECT_EQ(getOfUpdateSizeRes, (54 + 18) * (37 - 15));

    auto getGreaterThanUpdateSizeRes = tree.rangeGet(5, 40);
    EXPECT_EQ(getGreaterThanUpdateSizeRes, 54 * (40 - 5) + 18 * (37 - 15));

    auto intersectingGet1Res = tree.rangeGet(5, 30);
    EXPECT_EQ(intersectingGet1Res, 54 * (30 - 5) + 18 * (30 - 15));

    auto intersectingGet2Res = tree.rangeGet(30, 40);
    EXPECT_EQ(intersectingGet2Res, 54 * (40 - 30) + 18 * (37 - 30));
}

TEST(LazySegmentTree, LongLongToInt) {
    auto tree = LazySegmentTree<long long, int>(-100500, 100500, 42);

    auto testVal = tree.get(17);
    EXPECT_EQ(testVal, 42);
}

TEST(LazySegmentTree, IntToLong) {
    auto tree =
        LazySegmentTree<int, long long>(-100500, 100500, 1234567890098765432);

    auto testVal = tree.get(17);
    EXPECT_EQ(testVal, 1234567890098765432);
}

TEST(LazySegmentTree, LongLongToLongLong) {
    auto tree =
        LazySegmentTree<long long, long long>(-100500, 100500, 1234567890098765432);

    auto testVal = tree.get(17);
    EXPECT_EQ(testVal, 1234567890098765432);
}

TEST(LazySegmentTree, LongLongToLongLongLadder) {
    const auto topBorder = 1000000ll;
    auto tree = LazySegmentTree<long long, long long>(0, topBorder, 0);

    tree.update(topBorder - topBorder / 2, topBorder, 1);
    tree.update(topBorder - topBorder / 4, topBorder, 10);
    tree.update(topBorder - topBorder / 8, topBorder, 100);
    tree.update(topBorder - topBorder / 16, topBorder, 1000);
    tree.update(topBorder - topBorder / 32, topBorder, 10000);
    tree.update(topBorder - topBorder / 64, topBorder, 100000);
    tree.update(topBorder - topBorder / 128, topBorder, 1000000);
    tree.update(topBorder - topBorder / 256, topBorder, 10000000);

    auto testVal1 = tree.get(topBorder - 1);
    EXPECT_EQ(testVal1, 11111111);
    auto testVal2 = tree.get(topBorder - topBorder / 32 - 1);
    EXPECT_EQ(testVal2, 1111);
}

TEST(LazySegmentTree, Set) {
    auto tree = LazySegmentTree<int, int>(0, 42, 21);
    tree.set(0, 37, 73);
    EXPECT_EQ(tree.get(0), 73);
    EXPECT_EQ(tree.get(15), 73);
    EXPECT_EQ(tree.get(37), 21);
    EXPECT_EQ(tree.get(40), 21);
}

TEST(LazySegmentTree, SetOnTheSameRange) {
    auto tree = LazySegmentTree<int, int>(0, 42, 21);
    tree.set(0, 37, 73);
    tree.set(0, 37, 37);
    EXPECT_EQ(tree.get(0), 37);
    EXPECT_EQ(tree.get(15), 37);
    EXPECT_EQ(tree.get(37), 21);
    EXPECT_EQ(tree.get(40), 21);
}






