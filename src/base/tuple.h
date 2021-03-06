// Use std::tuple as tuple type. This file contains helper functions for
// working with std::tuples.
// The functions DispatchToMethod and DispatchToFunction take a function pointer
// or instance and method pointer, and unpack a tuple into arguments to the
// call.
//
// Example usage:
//   // These two methods of creating a Tuple are identical.
//   std::tuple<int, const char*> tuple_a(1, "wee");
//   std::tuple<int, const char*> tuple_b = std::make_tuple(1, "wee");
//
//   void SomeFunc(int a, const char* b) { }
//   DispatchToFunction(&SomeFunc, tuple_a);  // SomeFunc(1, "wee")
//   DispatchToFunction(
//       &SomeFunc, std::make_tuple(10, "foo"));    // SomeFunc(10, "foo")
//
//   struct { void SomeMeth(int a, int b, int c) { } } foo;
//   DispatchToMethod(&foo, &Foo::SomeMeth, std::make_tuple(1, 2, 3));
//   // foo->SomeMeth(1, 2, 3);

#ifndef BASE_TUPLE_H_
#define BASE_TUPLE_H_

#include <stddef.h>
#include <tuple>

#include "base/bind_helpers.h"

namespace base {

// Index sequences
//
// Minimal clone of the similarly-named C++14 functionality.

template <size_t...>
struct IndexSequence {};

template <size_t... Ns>
struct MakeIndexSequenceImpl;


template <size_t... Ns>
struct MakeIndexSequenceImpl<0, Ns...> {
  using Type = IndexSequence<Ns...>;
};

template <size_t N, size_t... Ns>
struct MakeIndexSequenceImpl<N, Ns...>
    : MakeIndexSequenceImpl<N - 1, N - 1, Ns...> {};


// std::get() in <=libstdc++-4.6 returns an lvalue-reference for
// rvalue-reference of a tuple, where an rvalue-reference is expected.
template <size_t I, typename... Ts>
typename std::tuple_element<I, std::tuple<Ts...>>::type&& get(
    std::tuple<Ts...>&& t) {
  using ElemType = typename std::tuple_element<I, std::tuple<Ts...>>::type;
  return std::forward<ElemType>(std::get<I>(t));
}

template <size_t I, typename T>
auto get(T& t) -> decltype(std::get<I>(t)) {
  return std::get<I>(t);
}

template <size_t N>
using MakeIndexSequence = typename MakeIndexSequenceImpl<N>::Type;

// Dispatchers ----------------------------------------------------------------
//
// Helper functions that call the given method on an object, with the unpacked
// tuple arguments.  Notice that they all have the same number of arguments,
// so you need only write:
//   DispatchToMethod(object, &Object::method, args);
// This is very useful for templated dispatchers, since they don't need to know
// what type |args| is.

// Non-Static Dispatchers with no out params.

template <typename ObjT, typename Method, typename... Ts, size_t... Ns>
inline void DispatchToMethodImpl(const ObjT& obj,
                                 Method method,
                                 const std::tuple<Ts...>& arg,
                                 IndexSequence<Ns...>) {
  (obj->*method)(internal::Unwrap(std::get<Ns>(arg))...);
}

template <typename ObjT, typename Method, typename... Ts>
inline void DispatchToMethod(const ObjT& obj,
                             Method method,
                             const std::tuple<Ts...>& arg) {
  DispatchToMethodImpl(obj, method, arg, MakeIndexSequence<sizeof...(Ts)>());
}

// Static Dispatchers with no out params.

template <typename Function, typename... Ts, size_t... Ns>
inline void DispatchToFunctionImpl(Function function,
                                   const std::tuple<Ts...>& arg,
                                   IndexSequence<Ns...>) {
  (*function)(internal::Unwrap(std::get<Ns>(arg))...);
}

template <typename Function, typename... Ts>
inline void DispatchToFunction(Function function,
                               const std::tuple<Ts...>& arg) {
  DispatchToFunctionImpl(function, arg, MakeIndexSequence<sizeof...(Ts)>());
}

// Dispatchers with out parameters.

template <typename ObjT,
          typename Method,
          typename... InTs,
          typename... OutTs,
          size_t... InNs,
          size_t... OutNs>
inline void DispatchToMethodImpl(const ObjT& obj,
                                 Method method,
                                 const std::tuple<InTs...>& in,
                                 std::tuple<OutTs...>* out,
                                 IndexSequence<InNs...>,
                                 IndexSequence<OutNs...>) {
  (obj->*method)(internal::Unwrap(std::get<InNs>(in))...,
                 &std::get<OutNs>(*out)...);
}

template <typename ObjT, typename Method, typename... InTs, typename... OutTs>
inline void DispatchToMethod(const ObjT& obj,
                             Method method,
                             const std::tuple<InTs...>& in,
                             std::tuple<OutTs...>* out) {
  DispatchToMethodImpl(obj, method, in, out,
                       MakeIndexSequence<sizeof...(InTs)>(),
                       MakeIndexSequence<sizeof...(OutTs)>());
}

}  // namespace base

#endif  // BASE_TUPLE_H_
