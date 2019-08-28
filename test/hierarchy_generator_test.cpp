#include "lib/catch.hpp"
#include "src/hierarchy_generator.hpp"

#include <string>
#include <type_traits>

template<typename T>
struct holder
{
    T value;
};

template<typename T, class Base>
struct linear_holder : public Base 
{};


TEST_CASE("hierarchy_generator field test") {
    using test_list = TL::type_list<int, double, std::string>;
    TL::scatter_hierarchy<test_list, holder> test_obj;

    auto &int_holder = TL::field<int>(test_obj);
    int_holder.value = 1;
    REQUIRE(TL::field<int>(test_obj).value == 1);

    auto &str_holder = TL::field<std::string>(test_obj);
    str_holder.value = "this is a test string";
    REQUIRE(TL::field<std::string>(test_obj).value == "this is a test string");

    auto &double_holder = TL::field<double>(test_obj);
    double_holder.value = 3.14;
    REQUIRE(TL::field<double>(test_obj).value == Approx(3.14));
}


TEST_CASE("hierarchy_generator positional index field test") {
    using test_list = TL::type_list<int, int, double, std::string, std::string>;
    TL::scatter_hierarchy<test_list, holder> test_obj;

    auto &first = TL::field<0>(test_obj);
    first.value = 1;
    REQUIRE(first.value == 1);

    auto &second = TL::field<1>(test_obj);
    second.value = 2;
    REQUIRE(first.value == 1);
    REQUIRE(second.value == 2);

    auto &third = TL::field<2>(test_obj);
    third.value = 3.1444;
    REQUIRE(third.value == Approx(3.1444));

    auto &fourth = TL::field<3>(test_obj);
    fourth.value = "this is a fourth string";
    REQUIRE(fourth.value == "this is a fourth string");

    auto &fifth = TL::field<4>(test_obj);
    fifth.value = "this is a fifth string";
    REQUIRE(fifth.value == "this is a fifth string");
}


TEST_CASE("linear_hierarchy test") {
    using test_list = TL::type_list<int, int, double, std::string, std::string>;

    using first_derived = TL::linear_hierarchy<test_list, linear_holder>;
    using first_base = linear_holder<int, TL::linear_hierarchy<TL::type_list<int, double, std::string, std::string>, linear_holder>>;
    REQUIRE(std::is_base_of_v<first_base, first_derived>);

    using second_base = TL::linear_hierarchy<TL::type_list<int, double, std::string, std::string>, linear_holder>;
    REQUIRE(std::is_base_of_v<second_base, first_base>);

    using third_base = linear_holder<int, TL::linear_hierarchy<TL::type_list<double, std::string, std::string>, linear_holder>>;
    REQUIRE(std::is_base_of_v<third_base, second_base>);

    using fourth_base = TL::linear_hierarchy<TL::type_list<double, std::string, std::string>, linear_holder>;
    REQUIRE(std::is_base_of_v<fourth_base, third_base>);

    using fifth_base = linear_holder<double, TL::linear_hierarchy<TL::type_list<std::string, std::string>, linear_holder>>;
    REQUIRE(std::is_base_of_v<fifth_base, fourth_base>);

    using six_base = TL::linear_hierarchy<TL::type_list<std::string, std::string>, linear_holder>;
    REQUIRE(std::is_base_of_v<six_base, fifth_base>);

    using seven_base = linear_holder<std::string, TL::linear_hierarchy<TL::type_list<std::string>, linear_holder>>;
    REQUIRE(std::is_base_of_v<seven_base, six_base>);

    using eigth_base = TL::linear_hierarchy<TL::type_list<std::string>, linear_holder>;
    REQUIRE(std::is_base_of_v<eigth_base, seven_base>);

    using nine_base = linear_holder<std::string, TL::empty_type>;
    REQUIRE(std::is_base_of_v<nine_base, eigth_base>);
    REQUIRE(std::is_base_of_v<TL::empty_type, nine_base>);
}