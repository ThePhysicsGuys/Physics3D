#pragma once

#include <type_traits>

namespace P3D {
#define IS_SUBCLASS_OF(Child, BaseClass) typename std::enable_if<std::is_base_of<BaseClass, Child>::value>::type* = nullptr
#define IS_ARITHMETIC(Type) typename std::enable_if<std::is_arithmetic<Type>::value>::type* = nullptr

template <int I> struct choice : choice<I - 1> {};
template <> struct choice<0> {};
};