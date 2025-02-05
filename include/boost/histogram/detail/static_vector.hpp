// Copyright 2019 Hans Dembinski
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_HISTOGRAM_DETAIL_STATIC_VECTOR_HPP
#define BOOST_HISTOGRAM_DETAIL_STATIC_VECTOR_HPP

#include <algorithm>
#include <boost/throw_exception.hpp>
#include <stdexcept>

namespace boost {
namespace histogram {
namespace detail {

// A crude implementation of boost::container::static_vector.
// Like std::vector, but with static allocation up to a maximum capacity.
template <class T, std::size_t N>
class static_vector {
  // Cannot inherit from std::array, since this confuses span.
  static constexpr bool swap_element_is_noexcept() noexcept {
    using std::swap;
    return noexcept(swap(std::declval<T&>(), std::declval<T&>()));
  }

public:
  using element_type = T;
  using size_type = std::size_t;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using iterator = pointer;
  using const_iterator = const_pointer;

  static_vector() = default;

  explicit static_vector(std::size_t s) noexcept : size_(s) { assert(size_ <= N); }

  static_vector(std::size_t s, const T& value) noexcept(
      std::is_nothrow_assignable<T, const_reference>::value)
      : static_vector(s) {
    fill(value);
  }

  static_vector(std::initializer_list<T> il) noexcept(
      std::is_nothrow_assignable<T, const_reference>::value)
      : static_vector(il.size()) {
    std::copy(il.begin(), il.end(), data_);
  }

  reference at(size_type pos) noexcept {
    if (pos >= size()) BOOST_THROW_EXCEPTION(std::out_of_range{"pos is out of range"});
    return data_[pos];
  }

  const_reference at(size_type pos) const noexcept {
    if (pos >= size()) BOOST_THROW_EXCEPTION(std::out_of_range{"pos is out of range"});
    return data_[pos];
  }

  reference operator[](size_type pos) noexcept { return data_[pos]; }
  const_reference operator[](size_type pos) const noexcept { return data_[pos]; }

  reference front() noexcept { return data_[0]; }
  const_reference front() const noexcept { return data_[0]; }

  reference back() noexcept { return data_[size_ - 1]; }
  const_reference back() const noexcept { return data_[size_ - 1]; }

  pointer data() noexcept { return static_cast<pointer>(data_); }
  const_pointer data() const noexcept { return static_cast<const_pointer>(data_); }

  iterator begin() noexcept { return data_; }
  const_iterator begin() const noexcept { return data_; }

  iterator end() noexcept { return begin() + size_; }
  const_iterator end() const noexcept { return begin() + size_; }

  const_iterator cbegin() const noexcept { return data_; }
  const_iterator cend() const noexcept { return cbegin() + size_; }

  constexpr size_type max_size() const noexcept { return N; }
  size_type size() const noexcept { return size_; }
  bool empty() const noexcept { return size_ == 0; }

  void fill(const_reference value) noexcept(
      std::is_nothrow_assignable<T, const_reference>::value) {
    std::fill(begin(), end(), value);
  }

  void swap(static_vector& other) noexcept(swap_element_is_noexcept()) {
    using std::swap;
    const size_type s = (std::max)(size(), other.size());
    for (auto i = begin(), j = other.begin(), end = begin() + s; i != end; ++i, ++j)
      swap(*i, *j);
    swap(size_, other.size_);
  }

private:
  size_type size_ = 0;
  element_type data_[N];
};

template <class T, std::size_t N>
bool operator==(const static_vector<T, N>& a, const static_vector<T, N>& b) noexcept {
  return std::equal(a.begin(), a.end(), b.begin(), b.end());
}

template <class T, std::size_t N>
bool operator!=(const static_vector<T, N>& a, const static_vector<T, N>& b) noexcept {
  return !(a == b);
}

} // namespace detail
} // namespace histogram
} // namespace boost

namespace std {
template <class T, std::size_t N>
void swap(
    ::boost::histogram::detail::static_vector<T, N>& a,
    ::boost::histogram::detail::static_vector<T, N>& b) noexcept(noexcept(a.swap(b))) {
  a.swap(b);
}
} // namespace std

#endif
