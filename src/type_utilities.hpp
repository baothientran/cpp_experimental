#ifndef TYPE_UTILITIES_HPP
#define TYPE_UTILITIES_HPP

namespace TL {

struct empty_type{};

template <int v>
struct int2type {
    static constexpr int value = v;
};

template <typename T>
struct type2type {
    using type = T;
};
}

#endif