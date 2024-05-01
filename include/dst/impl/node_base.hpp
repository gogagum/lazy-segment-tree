////////////////////////////////////////////////////////////////////////////////
// Copyright Georgy Guminov 2023-2024.
// Distributed under the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt
// or copySubtree at https://www.boost.org/LICENSE_1_0.txt)

#ifndef NODE_BASE_HPP
#define NODE_BASE_HPP

#include <concepts>
#include <memory>
#include <optional>

namespace dst::impl {

template <class Derived>
class BaseNode;

////////////////////////////////////////////////////////////////////////////////
/// \brief The BaseNode class
///
template <template <class...> class Derived, class T, class UpdateT,
          class Allocator>
class BaseNode<Derived<T, UpdateT, Allocator>> {
 protected:
  using This_ = BaseNode<Derived<T, UpdateT, Allocator>>;
  using Derived_ = Derived<T, UpdateT, Allocator>;
  using AllocForDerived_ =
      std::allocator_traits<Allocator>::template rebind_alloc<Derived_>;
  using AllocTraits_ = std::allocator_traits<AllocForDerived_>;

 public:
  BaseNode() = default;

  template <class T1>
  void setOrConstructValue(T1&& val, AllocForDerived_& alloc) {
    if (isLeaf()) {
      getValue() = std::forward<T1>(val);
    } else {
      clearChildren(alloc);
      std::construct_at(getValuePtr(), std::forward<T1>(val));
    }
  }

  /**
   * @brief Get const value reference.
   * @return value reference.
   */
  T& getValue() {
    return *std::launder(reinterpret_cast<T*>(&buffer_));  // NOLINT
  }

  [[nodiscard]] const T& getValue() const {
    return *std::launder(reinterpret_cast<T const*>(&buffer_));  // NOLINT
  }

  T* getValuePtr() {
    return std::addressof(getValue());
  }

  [[nodiscard]] bool isLeaf() const {
    return !ptr;
  }

  static void copyChildren(const Derived_& src, Derived_* dest,
                           AllocForDerived_& nodeAllocator) {
    Derived_::copySubtree(*src.getLeft(), dest->getLeft(), nodeAllocator);
    Derived_::copySubtree(*src.getRight(), dest->getRight(), nodeAllocator);
  }

  static void copySubtree(const Derived_& src, Derived_* dest,
                          AllocForDerived_& nodeAllocator) {
    if (dest->isLeaf()) {
      if (src.isLeaf()) {
        dest->getValue() = src.getValue();  // That's all!
      } else {
        std::destroy_at(dest->getValuePtr());
        // dest is like newly created
        copyChildrenToNewlyCreated(src, dest, nodeAllocator);
      }
    } else {
      if (src.isLeaf()) {
        dest->clearChildren(nodeAllocator);
        // Dest must not have value here
        std::construct_at(dest->getValuePtr(), src.getValue());
      } else {
        // No values in both, both have children
        copyChildren(src, dest, nodeAllocator);
      }
    }
  }

  static void copyToNewlyCreated(const Derived_& src, Derived_* dest,
                                 AllocForDerived_& nodeAlloc) {
    assert(dest->isLeaf());
    if (src.isLeaf()) {
      std::construct_at(dest->getValuePtr(), src.getValue());
    } else {
      copyChildrenToNewlyCreated(src, dest, nodeAlloc);
    }
  }

  static void copyChildrenToNewlyCreated(const Derived_& src, Derived_* dest,
                                         AllocForDerived_& nodeAlloc) {
    assert(dest->isLeaf());
    assert(!src.isLeaf());
    dest->initEmptyChildren(nodeAlloc);
    Derived_::copyToNewlyCreated(*src.getLeft(), dest->getLeft(), nodeAlloc);
    Derived_::copyToNewlyCreated(*src.getRight(), dest->getRight(), nodeAlloc);
  }

  void initEmptyChildren(AllocForDerived_& alloc) {
    ptr = AllocTraits_::allocate(alloc, 2);
    AllocTraits_::construct(alloc, getLeft());
    AllocTraits_::construct(alloc, getRight());
  }

  void initChildrenSiftingValue(AllocForDerived_& allocator);

  void clearChildren(AllocForDerived_& allocator);

  [[nodiscard]] Derived_* getLeft() const {
    return ptr;
  }

  [[nodiscard]] Derived_* getRight() const {
    return ptr + 1;  // NOLINT
  }

 public:
  Derived_* ptr{nullptr};  // NOLINT

 protected:
  ~BaseNode();

 private:
  void destructChildrenRecursive_();

 private:
  alignas(T) char buffer_[sizeof(T)];  // NOLINT
};

////////////////////////////////////////////////////////////////////////////////
template <template <class...> class Derived, class T, class UpdateT,
          class Allocator>
void BaseNode<Derived<T, UpdateT, Allocator>>::initChildrenSiftingValue(
    AllocForDerived_& allocator) {
  assert(isLeaf() && "Can only init children for a leaf.");
  auto* const nodesPtr = AllocTraits_::allocate(allocator, 2);  // Can throw
  ptr = nodesPtr;
  std::construct_at(getLeft());
  try {
    std::construct_at(getLeft()->getValuePtr(), getValue());
  } catch (...) {
    std::destroy_at(getLeft());
    AllocTraits_::deallocate(allocator, ptr, 2);
    ptr = nullptr;
    throw;
  }
  std::construct_at(getRight());
  try {
    if constexpr (std::move_constructible<T>) {
      std::construct_at(getRight()->getValuePtr(), std::move(getValue()));
    } else {
      std::construct_at(getRight()->getValuePtr(), getValue());
    }
  } catch (...) {
    std::destroy_at(getLeft()->getValuePtr());
    std::destroy_at(getLeft());
    std::destroy_at(getRight());
    AllocTraits_::deallocate(allocator, ptr, 2);
    ptr = nullptr;
    throw;
  }
  std::destroy_at(getValuePtr());
}

////////////////////////////////////////////////////////////////////////////////
template <template <class...> class Derived, class T, class UpdateT,
          class Allocator>
inline void BaseNode<Derived<T, UpdateT, Allocator>>::clearChildren(
    AllocForDerived_& allocator) {
  if (!getLeft()->isLeaf()) {
    getLeft()->clearChildren(allocator);
  } else {
    std::destroy_at(getLeft()->getValuePtr());
  }
  if (!getRight()->isLeaf()) {
    getRight()->clearChildren(allocator);
  } else {
    std::destroy_at(getRight()->getValuePtr());
  }
  AllocTraits_::destroy(allocator, getRight());
  AllocTraits_::destroy(allocator, getLeft());
  AllocTraits_::deallocate(allocator, ptr, 2);
  ptr = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template <template <class...> class Derived, class T, class UpdateT,
          class Allocator>
BaseNode<Derived<T, UpdateT, Allocator>>::~BaseNode() {
  assert(isLeaf());
}

}  // namespace dst::impl

#endif  // NODE_BASE_HPP
