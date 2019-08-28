#ifndef HIERARCHY_GENERATOR_HPP
#define HIERARCHY_GENERATOR_HPP

#include "type_lists.hpp"
#include "type_utilities.hpp"

namespace TL {

namespace secret {
    template<typename... Args>
    class scatter_hierarchy_tag;
}

// scatter_hierarchy definition
template<class TList, template<class> class Unit>
class scatter_hierarchy;

template<template<class> class Unit>
class scatter_hierarchy<empty_list, Unit>
{
public:
    template<typename HelperT>
    struct rebind {
        using type = Unit<HelperT>;
    };
};

template<class T, template<class> class Unit>
class scatter_hierarchy
    : public Unit<T>
{
public:
    template<typename HelperT>
    struct rebind {
        using type = Unit<HelperT>;
    };
};

template<template<class> class Unit, typename T, typename... Args>
class scatter_hierarchy<secret::scatter_hierarchy_tag<T, Args...>, Unit>
    : public scatter_hierarchy<T, Unit> 
{};

template<template<class> class Unit, typename T, typename... Args>
class scatter_hierarchy<type_list<T, Args...>, Unit>
    : public scatter_hierarchy<secret::scatter_hierarchy_tag<T, Args...>, Unit>,
      public scatter_hierarchy<type_list<Args...>, Unit>
{
public:
    using left_base = scatter_hierarchy<secret::scatter_hierarchy_tag<T, Args...>, Unit>;
    using right_base = scatter_hierarchy<type_list<Args...>, Unit>;

    template<int i>
    struct at {
        using type = Unit<typename get<i, type_list<T, Args...>>::type>;
    };

    template<typename HelperT>
    struct rebind {
        using type = Unit<HelperT>;
    };
};

// retrieve the field of scatter_hierarchy by type
template<typename T, typename H>
typename H::template rebind<T>::type& field(H &obj) {
    return obj;
}

// retrieve the field of scatter_hierarchy classes by positional index
template<typename T, typename H>
T &field_helper(H &obj, type2type<T>, int2type<0>) {
    typename H::left_base &subobj = obj;
    return subobj;
}

template<int i, typename T, typename H>
T &field_helper(H &obj, type2type<T> tt, int2type<i>) {
    typename H::right_base &subobj = obj;
    return field_helper(subobj, tt, int2type<i-1>());
}

template<int i, typename H>
typename H::template at<i>::type &field(H &obj) {
    using T = typename H::template at<i>::type;
    return field_helper(obj, type2type<T>(), int2type<i>());
}

// linear hierarchy definition
template<class TList, template<class AtomicType, class Base> class Unit, class Root = empty_type>
class linear_hierarchy;

template<template<class, class> class Unit, typename T, class Root>
class linear_hierarchy<type_list<T>, Unit, Root>
    : public Unit<T, Root>
{};

template<template<class, class> class Unit, class Root, typename T, typename... Args>
class linear_hierarchy<type_list<T, Args...>, Unit, Root>
    : public Unit<T, linear_hierarchy<type_list<Args...>, Unit, Root>>
{};

}

#endif