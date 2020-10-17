#pragma once

#include <type_traits>

/**
 * Determine if the given type is a templated type
 */

template<typename>
struct is_template : std::false_type {};

template<template<typename...> typename Type, typename... Arguments>
struct is_template<Type<Arguments...>> : std::true_type {};


/**
 * Determine if the given types are unique
 */

template<typename...>
inline constexpr auto unique_types = std::true_type {};

template<typename T, typename... Rest>
inline constexpr auto unique_types<T, Rest...> = std::bool_constant<(!std::is_same_v<T, Rest> && ...) && unique_types<Rest...>> {};