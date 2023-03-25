#ifndef DYNAMIC_SEGMENT_TREE_RANGE_GET_VARIATION_BASE_HPP
#define DYNAMIC_SEGMENT_TREE_RANGE_GET_VARIATION_BASE_HPP

#include <dst/concepts.hpp>
#include <dst/disable_operations.hpp>

namespace dst::impl {

////////////////////////////////////////////////////////////////////////////////
// Init variation                                                             //
////////////////////////////////////////////////////////////////////////////////
template <class KeyT, class ValueT, class GetValueT, class GetInit>
class DynamicSegmentTreeRangeGetInitVariationBase;

template <class KeyT, class ValueT, class GetValueT>
class DynamicSegmentTreeRangeGetInitVariationBase<KeyT, ValueT,
                                                  GetValueT, NoRangeGetOp> {
protected:
    explicit DynamicSegmentTreeRangeGetInitVariationBase(NoRangeGetOp) {};
};

////////////////////////////////////////////////////////////////////////////////
template <class KeyT,
          class ValueT,
          class GetValueT,
          dst::conc::ValueGetInitializer<ValueT, GetValueT> SegGetInit>
class DynamicSegmentTreeRangeGetInitVariationBase<KeyT, ValueT,
                                                  GetValueT, SegGetInit> {
protected:
    explicit DynamicSegmentTreeRangeGetInitVariationBase(
        const SegGetInit& segInit)
        : segInitializer_(segInit) {}
protected:

    GetValueT initGet_([[maybe_unused]] KeyT /*begin*/,
                       [[maybe_unused]] KeyT /*end*/,
                       const ValueT& val) const {
        return segInitializer_(val);
    }

protected:
    const SegGetInit segInitializer_;
};

////////////////////////////////////////////////////////////////////////////////
template <class KeyT,
          class ValueT,
          class GetValueT,
          dst::conc::ValueAndBordersGetInitializer<
              ValueT, KeyT, GetValueT> SegGetInit>
class DynamicSegmentTreeRangeGetInitVariationBase<KeyT, ValueT,
                                                  GetValueT, SegGetInit> {
protected:
    explicit DynamicSegmentTreeRangeGetInitVariationBase(
        const SegGetInit& segInit)
        : segInitializer_(segInit) {}
protected:
    GetValueT initGet_(KeyT begin, KeyT end, const ValueT& val) const {
        return segInitializer_(val, begin, end);
    }
protected:
    const SegGetInit segInitializer_;
};

////////////////////////////////////////////////////////////////////////////////
// Get variation                                                              //
////////////////////////////////////////////////////////////////////////////////
template <class KeyT, class GetValueT, class GetComb>
class DynamicSegmentTreeRangeGetCombineVariationBase;

////////////////////////////////////////////////////////////////////////////////
template <class KeyT, class GetValueT>
class DynamicSegmentTreeRangeGetCombineVariationBase<KeyT, GetValueT,
                                                     NoRangeGetOp> {
protected:
    explicit DynamicSegmentTreeRangeGetCombineVariationBase(NoRangeGetOp) {}
};

////////////////////////////////////////////////////////////////////////////////
template <class KeyT,
          class GetValueT,
          dst::conc::ValueGetCombiner<GetValueT> SegGetComb>
class DynamicSegmentTreeRangeGetCombineVariationBase<KeyT, GetValueT,
                                                     SegGetComb> {
protected:
    explicit DynamicSegmentTreeRangeGetCombineVariationBase(const SegGetComb& segComb)
        : segCombiner_(segComb) {}
protected:
    GetValueT combineGet_(const GetValueT& leftValue,
                          const GetValueT& rightValue,
                          [[maybe_unused]] KeyT /*leftBegin*/,
                          [[maybe_unused]] KeyT /*separation*/,
                          [[maybe_unused]] KeyT /*rightEnd*/) const {
        return segCombiner_(leftValue, rightValue);
    }
protected:
    const SegGetComb segCombiner_;
};

////////////////////////////////////////////////////////////////////////////////
template <class KeyT,
          class GetValueT,
          dst::conc::ValueAndBordersGetCombiner<GetValueT, KeyT> SegGetComb>
class DynamicSegmentTreeRangeGetCombineVariationBase<KeyT, GetValueT,
                                                     SegGetComb> {
protected:
    explicit DynamicSegmentTreeRangeGetCombineVariationBase(
        const SegGetComb& segComb)
        : segCombiner_(segComb) {}
protected:
    GetValueT combineGet_(const GetValueT& leftValue,
                          const GetValueT& rightValue,
                          KeyT leftBegin,
                          KeyT separation,
                          KeyT rightEnd) const {
        return segCombiner_(leftValue, rightValue,
                            leftBegin, separation, rightEnd);
    }
protected:
    const SegGetComb segCombiner_;
};

}

#endif // DYNAMIC_SEGMENT_TREE_RANGE_GET_VARIATION_BASE_HPP
