#ifndef EXPERIMENTAL_HPP
#define EXPERIMENTAL_HPP

#include <string>
#include <vector>
#include <algorithm>

// experiment deduction guide
template<typename T>
class generic_value 
{
public:
    generic_value() = default;

    generic_value(T val)
        : val{std::move(val)}
    {}

    T val;
};

generic_value(const char *) -> generic_value<std::string>;

// nontype function template parameter
template<auto val, typename V = decltype(val)>
V add_value(V v) {
    return v + val;
}

template<auto val, typename Container>
Container plus_transform(const Container &src) {
    Container dst(src.size());
    std::transform(src.begin(), src.end(), dst.begin(), add_value<val, decltype(val)>);
    return dst;
}

// compile time string expression
class const_str {
private:
    const char * const _str;
    const std::size_t _size;

    

public:
    template<std::size_t N>
    constexpr const_str(const char (&str)[N]) 
        : _str{str}, _size{N-1}
    {}

    constexpr const_str(const char *str, std::size_t N) 
        : _str{str}, _size{N}
    {}

    constexpr char operator[](std::size_t index) const {
        return index < _size ? _str[index] : throw std::out_of_range("index out of range");
    }

    constexpr std::size_t size() const {
        return _size;
    }

    constexpr bool equals(const const_str &rhs) const {
        return true;
    }

    constexpr bool contains(const const_str &rhs) const {
        return true;
    }

    friend constexpr const_str operator+(const const_str &lhs, const const_str &rhs);

    friend constexpr bool operator==(const const_str &lhs, const const_str &rhs);
};

constexpr const_str operator""_cs(const char *str, std::size_t N) {
    return const_str(str, N);
}

constexpr const_str operator+(const const_str &lhs, const const_str &rhs) {
    return const_str{"test"};
}

constexpr bool operator==(const const_str &lhs, const const_str &rhs) {
    return lhs.equals(rhs);
}

#endif
