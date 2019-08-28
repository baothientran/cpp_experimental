#include "src/experimental.hpp"
#include "lib/catch.hpp"

#include <type_traits>
#include <iostream>

TEST_CASE("Language rule: Cannot copy initialize an object by passing string literal to a construct expecting a std::string") {
    using namespace std::string_literals;
    generic_value exp = "this is a test string"s;
    // generic_value err = "this is a test string"; // this should be flagged as error in gcc, but MSVC still compile so...
    REQUIRE(std::is_same_v<decltype(exp), generic_value<std::string>>);
    REQUIRE(exp.val == "this is a test string");
}

TEST_CASE("Nontype function template parameters") {
    std::vector<int> inttest = { 1, 2, 3, 4, 5 };
    auto dst = plus_transform<5>(inttest);
    std::vector<int> expected = {6, 7, 8, 9, 10};
    REQUIRE(dst == expected);
}

TEST_CASE("const_str test") {
    constexpr const_str lhs("this is a test");
    constexpr const_str rhs = "this is a test"_cs;
    constexpr const_str res = lhs + rhs;
}
