// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14

// <variant>

// template <class ...Types> class variant;

// variant& operator=(variant&&) noexcept(see below);

#include <type_traits>
#include <variant>

#include "test_macros.h"

struct NTMoveAssign {
  constexpr NTMoveAssign(int v) : value(v) {}
  NTMoveAssign(const NTMoveAssign &) = default;
  NTMoveAssign(NTMoveAssign &&) = default;
  NTMoveAssign &operator=(const NTMoveAssign &that) = default;
  NTMoveAssign &operator=(NTMoveAssign &&that) {
    value = that.value;
    that.value = -1;
    return *this;
  };
  int value;
};

static_assert(!std::is_trivially_move_assignable<NTMoveAssign>::value, "");
static_assert(std::is_move_assignable<NTMoveAssign>::value, "");

struct TMoveAssign {
  constexpr TMoveAssign(int v) : value(v) {}
  TMoveAssign(const TMoveAssign &) = delete;
  TMoveAssign(TMoveAssign &&) = default;
  TMoveAssign &operator=(const TMoveAssign &) = delete;
  TMoveAssign &operator=(TMoveAssign &&) = default;
  int value;
};

static_assert(std::is_trivially_move_assignable<TMoveAssign>::value, "");

struct TMoveAssignNTCopyAssign {
  constexpr TMoveAssignNTCopyAssign(int v) : value(v) {}
  TMoveAssignNTCopyAssign(const TMoveAssignNTCopyAssign &) = default;
  TMoveAssignNTCopyAssign(TMoveAssignNTCopyAssign &&) = default;
  TMoveAssignNTCopyAssign &operator=(const TMoveAssignNTCopyAssign &that) {
    value = that.value;
    return *this;
  }
  TMoveAssignNTCopyAssign &operator=(TMoveAssignNTCopyAssign &&) = default;
  int value;
};

static_assert(std::is_trivially_move_assignable_v<TMoveAssignNTCopyAssign>);

void test_move_assignment_sfinae() {
  {
    using V = std::variant<int, long>;
    static_assert(std::is_trivially_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, NTMoveAssign>;
    static_assert(!std::is_trivially_move_assignable<V>::value, "");
    static_assert(std::is_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, TMoveAssign>;
    static_assert(std::is_trivially_move_assignable<V>::value, "");
  }
  {
    using V = std::variant<int, TMoveAssignNTCopyAssign>;
    static_assert(std::is_trivially_move_assignable<V>::value, "");
  }
}

template <typename T> struct Result { size_t index; T value; };

void test_move_assignment_same_index() {
  {
    struct {
      constexpr Result<int> operator()() const {
        using V = std::variant<int>;
        V v(43);
        V v2(42);
        v = std::move(v2);
        return {v.index(), std::get<0>(v)};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 0);
    static_assert(result.value == 42);
  }
  {
    struct {
      constexpr Result<long> operator()() const {
        using V = std::variant<int, long, unsigned>;
        V v(43l);
        V v2(42l);
        v = std::move(v2);
        return {v.index(), std::get<1>(v)};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1);
    static_assert(result.value == 42l);
  }
  {
    struct {
      constexpr Result<int> operator()() const {
        using V = std::variant<int, TMoveAssign, unsigned>;
        V v(std::in_place_type<TMoveAssign>, 43);
        V v2(std::in_place_type<TMoveAssign>, 42);
        v = std::move(v2);
        return {v.index(), std::get<1>(v).value};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1);
    static_assert(result.value == 42);
  }
}

void test_move_assignment_different_index() {
  {
    struct {
      constexpr Result<long> operator()() const {
        using V = std::variant<int, long, unsigned>;
        V v(43);
        V v2(42l);
        v = std::move(v2);
        return {v.index(), std::get<1>(v)};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1);
    static_assert(result.value == 42l);
  }
  {
    struct {
      constexpr Result<long> operator()() const {
        using V = std::variant<int, TMoveAssign, unsigned>;
        V v(std::in_place_type<unsigned>, 43);
        V v2(std::in_place_type<TMoveAssign>, 42);
        v = std::move(v2);
        return {v.index(), std::get<1>(v).value};
      }
    } test;
    constexpr auto result = test();
    static_assert(result.index == 1);
    static_assert(result.value == 42);
  }
}

int main() {
  test_move_assignment_same_index();
  test_move_assignment_different_index();
  test_move_assignment_sfinae();
}