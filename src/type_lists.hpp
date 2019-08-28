#ifndef TYPE_LISTS_HPP
#define TYPE_LISTS_HPP

#include <type_traits>

namespace TL {

// define type_list
template<typename... Args>
class type_list;

// define empty type_list
using empty_list = type_list<>;

// length of a type_list
template<typename... Args>
struct length;

template<template <class...> typename Va>
struct length<Va<>>{
    static constexpr unsigned value = 0;
};

template<template<typename...> typename Va, typename T, typename... Args> 
struct length<Va<T, Args...>> {
    static constexpr unsigned value = 1 + length<Va<Args...>>::value;
};

// get a type at index in the type_list
template<unsigned int N, typename... Args>
struct get;

template<template <typename...> typename Va, typename T, typename... Args>
struct get<0, Va<T, Args...>> {
    using type = T;
};

template<unsigned int N, template<typename...> typename Va, typename T, typename... Args>
struct get<N, Va<T, Args...>> {
    using type = typename get<N-1, Va<Args...>>::type;
};

// index of a type in the type_list
template<typename... Args>
struct index_of;

template<template<typename...> typename Va, typename N>
struct index_of<N, Va<>> {
    static constexpr int value = -1;
};

template<template<typename...> typename Va, typename N, typename... Args>
struct index_of<N, Va<N, Args...>> {
    static constexpr int value = 0;
};

template<template<typename...> typename Va, typename N, typename T, typename... Args>
struct index_of<N, Va<T, Args...>> {
private:
    static constexpr int rest_value = index_of<N, Va<Args...>>::value;

public:
    static constexpr int value =  rest_value != -1 ? 1 + rest_value : -1;
};

// append a type or type_list to the type_list
template<typename... Args>
struct append;

template<template<typename...> typename Va, typename N, typename... Args>
struct append<N, Va<Args...>> {
    using type = Va<Args..., N>;
};

// push a type in front of type_list
template<typename... Args>
struct push_front;

template<template<typename...> typename Va, typename N, typename... Args>
struct push_front<N, Va<Args...>> {
    using type = Va<N, Args...>;
};

// flatten a type_list
template<typename... Args>
struct flatten;

template<template<typename...> typename Va>
struct flatten<Va<>> {
    using type = Va<>;
};

template<template<typename...> typename Va, typename N, typename... Args>
struct flatten<Va<N, Args...>> {
private:
    using rest = typename flatten<Va<Args...>>::type;

public:
    using type = typename push_front<N, rest>::type;
};

template<template<typename...> typename Va, typename... Ns, typename... Args>
struct flatten<Va<Va<Ns...>, Args...>> {
    using type = typename flatten<Va<Ns..., Args...>>::type;
};

// erase a type from type_list
template<typename... Args>
struct erase;

template<template<typename...> typename Va, typename N>
struct erase<N, Va<>> {
    using type = Va<>;
};

template<template<typename...> typename Va, typename N, typename... Args>
struct erase<N, Va<N, Args...>> {
    using type = Va<Args...>;
};

template<template<typename...> typename Va, typename N, typename T, typename... Args>
struct erase<N, Va<T, Args...>>{
    using type = typename push_front<T, typename erase<N, Va<Args...>>::type>::type;
};

// erase all occurence of a type from type_list
template<typename... Args>
struct erase_all;

template<template<typename...> typename Va, typename N>
struct erase_all<N, Va<>> {
    using type = empty_list;
};

template<template<typename...> typename Va, typename N, typename... Args>
struct erase_all<N, Va<N, Args...>> {
    using type = typename erase_all<N, Va<Args...>>::type;
};

template<template<typename...> typename Va, typename N, typename T, typename... Args>
struct erase_all<N, Va<T, Args...>>{
    using type = typename push_front<T, typename erase_all<N, Va<Args...>>::type>::type;
};

// remove duplicate types from the type_list
template<typename... Args>
struct erase_duplicate;

template<template<typename...> typename Va>
struct erase_duplicate<Va<>> {
    using type = Va<>;
};

template<template<typename...> typename Va, typename N, typename... Args>
struct erase_duplicate<Va<N, Args...>>{
private:
    using L1 = typename erase_duplicate<Va<Args...>>::type;
    using L2 = typename erase<N, L1>::type;

public:
    using type = typename push_front<N, L2>::type;
};

// replace type in type_list
template<typename... Args>
struct replace;

template<template<typename...> typename Va, typename N, typename R>
struct replace<N, R, Va<>>{
    using type = Va<>;
};

template<template<typename...> typename Va, typename N, typename R, typename... Args>
struct replace<N, R, Va<N, Args...>> {
    using type = Va<R, Args...>;
};

template<template<typename...> typename Va, typename N, typename R, typename T, typename... Args>
struct replace<N, R, Va<T, Args...>> {
private:
    using rest = typename replace<N, R, Va<Args...>>::type;
public:
    using type = typename push_front<T, rest>::type;
};

// replace all type in type_list
template<typename... Args>
struct replace_all;

template<template<typename...> typename Va, typename N, typename R>
struct replace_all<N, R, Va<>> {
    using type = Va<>;
};

template<template<typename...> typename Va, typename R, typename N, typename... Args>
struct replace_all<N, R, Va<N, Args...>>{
private:
    using rest = typename replace_all<N, R, Va<Args...>>::type;
public:
    using type = typename push_front<R, rest>::type; 
};

template<template<typename...> typename Va, typename R, typename N, typename T, typename... Args>
struct replace_all<N, R, Va<T, Args...>>{
private:
    using rest = typename replace_all<N, R, Va<Args...>>::type;
public:
    using type = typename push_front<T, rest>::type;
};

// find the most derived in the type_list
template<typename... Args>
struct most_derived;

template<template<typename...> typename Va, typename Base>
struct most_derived<Base, Va<>>{
    using type = Base;
};

template<template<typename...> typename Va, typename Base, typename T, typename... Args>
struct most_derived<Base, Va<T, Args...>> {
private:
    using D = typename most_derived<Base, Va<Args...>>::type;
public:
    using type = std::conditional_t<std::is_base_of_v<D, T>, T, D>;
};

// order the type_list so that the most derived in the front of the list
template<typename... Args>
struct derived_to_front;

template<template<typename...> typename Va>
struct derived_to_front<Va<>>{
    using type = Va<>;
};

template<template<typename...> typename Va, typename T, typename... Args>
struct derived_to_front<Va<T, Args...>> {
private:
    using D = typename most_derived<T, Va<Args...>>::type;
    using rest = typename derived_to_front<typename replace<D, T, Va<Args...>>::type>::type;

public:
    using type = typename push_front<D, rest>::type;
};

};

#endif