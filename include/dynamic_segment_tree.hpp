#ifndef DYNAMIC_SEGMENT_TREE_HPP
#define DYNAMIC_SEGMENT_TREE_HPP

#include <concepts>
#include <functional>

#include <node.hpp>

namespace dst{

template <class T, class  ValueT>
concept ValueSegmentCombiner =
    requires(const T& segComb, const ValueT& left, const ValueT& right) {
         { segComb(left, right) } -> std::same_as<ValueT>;
    };

template <class T, class ValueT, class KeyT>
concept ValueAndLengthCombiner =
    requires(const T& segComb, const ValueT& left, const ValueT& right,
             KeyT leftLength, KeyT rightLength) {
        { segComb(left, right, leftLength, rightLength) } -> std::same_as<ValueT>;
    };

template <class T, class ValueT, class KeyT>
concept SegmentCombiner = ValueSegmentCombiner<T, ValueT>
                          || ValueAndLengthCombiner<T, ValueT, KeyT>;

template <class T, class  ValueT>
concept ValueSegmentInitializer =
    requires(const T& segInit, const ValueT& value) {
         { segInit(value) } -> std::convertible_to<ValueT>;
    };

template <class T, class ValueT, class KeyT>
concept ValueAndLengthSegmentInitializer =
    requires(const T& segInit, const ValueT& value, KeyT length) {
        { segInit(value, length) } -> std::convertible_to<ValueT>;
    };

template <class T, class ValueT, class KeyT>
concept SegmentInitializer = ValueSegmentInitializer<T, ValueT>
                          || ValueAndLengthSegmentInitializer<T, ValueT, KeyT>;


template <std::integral KeyT,
          class ValueT,
          SegmentCombiner<ValueT, KeyT> SegCombiner,
          SegmentInitializer<ValueT, KeyT> SegInitializer,
          class UpdateOp,
          class Allocator = std::allocator<ValueT>>
class DynamicSegmentTree{
private:
    using _Node = Node<ValueT, UpdateOp, Allocator>;

public:
    DynamicSegmentTree(KeyT start, KeyT end, const ValueT& value);
    template <class _UpdateOp = UpdateOp> requires (!std::is_same_v<UpdateOp, void>)
    void update(KeyT begin, KeyT end, const ValueT& toUpdate);
    void set(KeyT begin, KeyT end, const ValueT& toSet);
    ValueT get(KeyT key) const;
    ValueT rangeGet(KeyT begin, KeyT end) const;
private:
    void _updateImpl(KeyT start, KeyT end, KeyT currStart, KeyT currEnd,
                     _Node* currNode, const ValueT& toUpdate);
    void _setImpl(KeyT start, KeyT end, KeyT currStart, KeyT currEnd,
                     _Node* currNode, const ValueT& toUpdate);
    ValueT _getImpl(KeyT key, KeyT currBegin, KeyT currEnd,
                    _Node* currNode) const;
    ValueT _rangeGetImpl(KeyT begin, KeyT end,
                         KeyT currBegin, KeyT currEnd,
                         _Node* currNode) const;
private:
    const SegCombiner _segCombiner{};
    const SegInitializer _segInitializer{};
    mutable _Node _rootNode;
    KeyT _begin;
    KeyT _end;
};

template <std::integral KeyT,
          class ValueT,
          SegmentCombiner<ValueT, KeyT> SegCombiner,
          SegmentInitializer<ValueT, KeyT> SegmentInitializer,
          class UpdateOp,
          class Allocator>
DynamicSegmentTree<KeyT, ValueT, SegCombiner,
                   SegmentInitializer, UpdateOp, Allocator>::DynamicSegmentTree(
        KeyT begin, KeyT end, const ValueT& value)
    : _rootNode(value), _begin(begin), _end(end) {}

template <std::integral KeyT,
          class ValueT,
          SegmentCombiner<ValueT, KeyT> SegCombiner,
          SegmentInitializer<ValueT, KeyT> SegmentInitializer,
          class UpdateOp,
          class Allocator>
template <class _UpdateOp> requires (!std::is_same_v<UpdateOp, void>)
void DynamicSegmentTree<KeyT, ValueT, SegCombiner,
                        SegmentInitializer, UpdateOp, Allocator>::update(
        KeyT begin, KeyT end, const ValueT& toUpdate) {
    _updateImpl(begin, end, _begin, _end, &_rootNode, toUpdate);
}

template <std::integral KeyT,
          class ValueT,
          SegmentCombiner<ValueT, KeyT> SegCombiner,
          SegmentInitializer<ValueT, KeyT> SegmentInitializer,
          class UpdateOp,
          class Allocator>
void DynamicSegmentTree<KeyT, ValueT, SegCombiner, SegmentInitializer,
                        UpdateOp, Allocator>::set(
        KeyT begin, KeyT end, const ValueT& toUpdate) {
    _setImpl(begin, end, _begin, _end, &_rootNode, toUpdate);
}

template <std::integral KeyT,
          class ValueT,
          SegmentCombiner<ValueT, KeyT> SegCombiner,
          SegmentInitializer<ValueT, KeyT> SegmentInitializer,
          class UpdateOp,
          class Allocator>
ValueT DynamicSegmentTree<KeyT, ValueT, SegCombiner,
                          SegmentInitializer, UpdateOp, Allocator>::get(
        KeyT key) const {
    if (key >= _end || key < _begin) {
        return ValueT{};
    }
    return _getImpl(key, _begin, _end, &_rootNode);
}

template <std::integral KeyT,
          class ValueT,
          SegmentCombiner<ValueT, KeyT> SegCombiner,
          SegmentInitializer<ValueT, KeyT> SegmentInitializer,
          class UpdateOp,
          class Allocator>
ValueT DynamicSegmentTree<KeyT, ValueT, SegCombiner,
                          SegmentInitializer, UpdateOp, Allocator>::rangeGet(
        KeyT begin, KeyT end) const {
    return _rangeGetImpl(begin, end, _begin, _end, &_rootNode);
}

template <std::integral KeyT,
          class ValueT,
          SegmentCombiner<ValueT, KeyT> SegCombiner,
          SegmentInitializer<ValueT, KeyT> SegmentInitializer,
          class UpdateOp,
          class Allocator>
void DynamicSegmentTree<KeyT, ValueT, SegCombiner,
                        SegmentInitializer, UpdateOp, Allocator>::_updateImpl(
        KeyT begin, KeyT end, KeyT currBegin, KeyT currEnd,
        _Node* currNode, const ValueT& toUpdate) {
    if (begin >= currEnd || currBegin >= end) {
        return;
    }
    if (end >= currEnd && begin <= currBegin) {
        currNode->update(toUpdate);
        return;
    }
    const auto m = (currBegin + currEnd) / 2;
    currNode->siftDown();
    if (m >= currBegin + 1) {
        auto leftNodePtr = currNode->getLeft();
        _updateImpl(begin, end, currBegin, m, leftNodePtr, toUpdate);
    }
    if (currEnd >= m + 1) {
        auto rightNodePtr = currNode->getRight();
        _updateImpl(begin, end, m, currEnd, rightNodePtr, toUpdate);
    }
}

template <std::integral KeyT,
          class ValueT,
          SegmentCombiner<ValueT, KeyT> SegCombiner,
          SegmentInitializer<ValueT, KeyT> SegmentInitializer,
          class UpdateOp,
          class Allocator>
void DynamicSegmentTree<KeyT, ValueT, SegCombiner, SegmentInitializer,
                        UpdateOp, Allocator>::_setImpl(
        KeyT begin, KeyT end, KeyT currBegin, KeyT currEnd,
        _Node* currNode, const ValueT& toUpdate) {
    if (begin >= currEnd || currBegin >= end) {
        return;
    }
    if (end >= currEnd && begin <= currBegin) {
        currNode->setValue(toUpdate);
        return;
    }
    const auto m = (currBegin + currEnd) / 2;
    currNode->siftDown();
    if (m >= currBegin + 1) {
        auto leftNodePtr = currNode->getLeft();
        _setImpl(begin, end, currBegin, m, leftNodePtr, toUpdate);
    }
    if (currEnd >= begin + 1) {
        auto rightNodePtr = currNode->getRight();
        _setImpl(begin, end, m, currEnd, rightNodePtr, toUpdate);
    }
}

template <std::integral KeyT,
          class ValueT,
          SegmentCombiner<ValueT, KeyT> SegCombiner,
          SegmentInitializer<ValueT, KeyT> SegmentInitializer,
          class UpdateOp,
          class Allocator>
ValueT DynamicSegmentTree<KeyT, ValueT, SegCombiner,
                          SegmentInitializer, UpdateOp, Allocator>::_getImpl(
        KeyT key, KeyT currBegin, KeyT currEnd,
        _Node* currNode) const {
    if (currNode->isLeaf()) {
        return currNode->getValue();
    }
    currNode->siftDown();
    if (auto m = (currBegin + currEnd) / 2; key >= m) {
        _Node* visitedNode = currNode->getRight();
        return _getImpl(key, m, currEnd, visitedNode);
    } else {
        _Node* visitedNode = currNode->getLeft();
        return _getImpl(key, currBegin, m, visitedNode);
    }
}

template <std::integral KeyT,
          class ValueT,
          SegmentCombiner<ValueT, KeyT> SegCombiner,
          SegmentInitializer<ValueT, KeyT> SegmentInitializer,
          class UpdateOp,
          class Allocator>
ValueT
DynamicSegmentTree<KeyT, ValueT, SegCombiner,
                   SegmentInitializer, UpdateOp, Allocator>::_rangeGetImpl(
        KeyT begin, KeyT end, KeyT currBegin,
        KeyT currEnd, _Node* currNode) const {
    if (begin >= currEnd || currBegin >= end) {
        assert(false);
        return ValueT{};
    }
    if (end >= currEnd && begin <= currBegin && currNode->isLeaf()) {
        if constexpr (ValueSegmentInitializer<SegmentInitializer, ValueT>) {
            return _segInitializer(currNode->getValue());
        } else {
            return _segInitializer(currNode->getValue(), currEnd - currBegin);
        }
    }
    currNode->siftDown();
    const auto m = (currBegin + currEnd) / 2;

    _Node* rightNodePtr = currNode->getRight();
    _Node* leftNodePtr = currNode->getLeft();

    if (begin >= m) { // only right
        return _rangeGetImpl(begin, end, m, currEnd, rightNodePtr);
    }
    if (end <= m) { // only left
        return _rangeGetImpl(begin, end, currBegin, m, leftNodePtr);
    }

    const ValueT lVal = _rangeGetImpl(begin, end, currBegin, m, leftNodePtr);
    const ValueT rVal = _rangeGetImpl(begin, end, m, currEnd, rightNodePtr);

    if constexpr (ValueAndLengthCombiner<SegCombiner, ValueT, KeyT>) {
        const KeyT leftLength = m - begin;
        const KeyT rightLength = end - m;
        return _segCombiner(lVal, rVal, leftLength, rightLength);
    } else if constexpr (ValueSegmentCombiner<SegCombiner, ValueT>) {
        return _segCombiner(lVal, rVal);
    }
}

}

#endif // DYNAMIC_SEGMENT_TREE_HPP
