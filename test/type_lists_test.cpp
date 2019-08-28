#include "lib/catch.hpp"
#include "src/type_lists.hpp"

#include <string>
#include <vector>
#include <type_traits>
#include <iostream>


TEST_CASE("type_list length test") {
    REQUIRE(TL::length<TL::empty_list>::value == 0);
    REQUIRE(TL::length<TL::type_list<int, double>>::value == 2);
    REQUIRE(TL::length<TL::type_list<int, int>>::value == 2);
    REQUIRE(TL::length<TL::type_list<int, double, std::string>>::value == 3);
}


TEST_CASE("type_list get test") {
    REQUIRE(std::is_same_v<TL::get<0, TL::type_list<int, double, std::string, std::vector<int>>>::type, int>);
    REQUIRE(std::is_same_v<TL::get<1, TL::type_list<int, double, std::string, std::vector<int>>>::type, double>);
    REQUIRE(std::is_same_v<TL::get<2, TL::type_list<int, double, std::string, std::vector<int>>>::type, std::string>);
    REQUIRE(std::is_same_v<TL::get<3, TL::type_list<int, double, std::string, std::vector<int>>>::type, std::vector<int>>);

    TL::get<0, TL::type_list<int, double, std::string>>::type integerType = 1;
    TL::get<1, TL::type_list<int, double, std::string>>::type doubleType = 3.14;
    TL::get<2, TL::type_list<int, double, std::string>>::type stringType = "this is a test string";
    REQUIRE(integerType == 1);
    REQUIRE(doubleType == Approx(3.14));
    REQUIRE(stringType == "this is a test string");
}


TEST_CASE("type_list index_of test") {
    REQUIRE(TL::index_of<int, TL::type_list<int, double, std::string, std::vector<std::string>>>::value == 0);
    REQUIRE(TL::index_of<double, TL::type_list<int, double, std::string, std::vector<std::string>>>::value == 1);
    REQUIRE(TL::index_of<std::string, TL::type_list<int, double, std::string, std::vector<std::string>>>::value == 2);
    REQUIRE(TL::index_of<std::vector<std::string>, TL::type_list<int, double, std::string, std::vector<std::string>>>::value == 3);
    REQUIRE(TL::index_of<std::vector<int>, TL::type_list<int, double, std::string, std::vector<std::string>>>::value == -1);
}


TEST_CASE("type_list append test") {
    REQUIRE(std::is_same_v<TL::append<TL::empty_list, TL::empty_list>::type, TL::type_list<TL::empty_list>>);
    REQUIRE(std::is_same_v<TL::append<int, TL::empty_list>::type, TL::type_list<int>>);
    REQUIRE(std::is_same_v<TL::append<TL::type_list<int, double>, TL::empty_list>::type, TL::type_list<TL::type_list<int, double>>>);
    REQUIRE(std::is_same_v<TL::append<TL::empty_list, TL::type_list<int, double>>::type, TL::type_list<int, double, TL::empty_list>>);
    REQUIRE(std::is_same_v<TL::append<int, TL::type_list<double, std::string>>::type, TL::type_list<double, std::string, int>>);
    REQUIRE(std::is_same_v<TL::append<void, TL::type_list<double, std::string>>::type, TL::type_list<double, std::string, void>>);
    REQUIRE(std::is_same_v<TL::append<TL::empty_list, TL::type_list<double, std::string>>::type, TL::type_list<double, std::string, TL::empty_list>>);
    REQUIRE(std::is_same_v<TL::append<TL::type_list<std::string>, TL::type_list<double, std::string>>::type, TL::type_list<double, std::string, TL::type_list<std::string>>>);
}


TEST_CASE("type_list push_front test") {

}


TEST_CASE("type_list flatten test") {
    REQUIRE(std::is_same_v<TL::flatten<TL::type_list<TL::type_list<int, double>, std::string>>::type, TL::type_list<int, double, std::string>>);

    using test_list_1 = TL::type_list<
                                    TL::type_list<TL::type_list<int>, std::string>,
                                    int,
                                    double,
                                    std::string,
                                    TL::type_list<TL::type_list<TL::type_list<int>>, TL::type_list<int, double>>,
                                    TL::type_list<int, TL::type_list<int, std::string>>
                                >;
    using expected_list_1 = TL::type_list<
                                    int, std::string,
                                    int,
                                    double,
                                    std::string,
                                    int, int, double,
                                    int, int, std::string
                                >;
    REQUIRE(std::is_same_v<TL::flatten<test_list_1>::type, expected_list_1>);

    using test_list_2 = TL::type_list<
                                    TL::type_list<TL::type_list<TL::type_list<int>>, TL::type_list<int, double>>,
                                    int
                                >;
    using expected_list_2 = TL::type_list<
                                    int, int, double,
                                    int
                                >;
    REQUIRE(std::is_same_v<TL::flatten<test_list_2>::type, expected_list_2>);

    using test_list_3 = TL::type_list<
                                    TL::empty_list,
                                    TL::empty_list,
                                    TL::type_list<TL::type_list<TL::type_list<int>>, TL::type_list<int, double, TL::empty_list>>,
                                    TL::type_list<TL::type_list<TL::type_list<int>>, TL::type_list<int, TL::type_list<double>>,
                                    TL::empty_list>
                                >;
    using expected_list_3 = TL::type_list<
                                    int, int, double,
                                    int, int, double
                                >;
    REQUIRE(std::is_same_v<TL::flatten<test_list_3>::type, expected_list_3>);

    using test_list_4 = TL::type_list<
                                    int,
                                    TL::type_list<TL::type_list<TL::empty_list, int>, TL::type_list<int, double, TL::type_list<TL::empty_list>>>
                                >;
    using expected_list_4 = TL::type_list<
                                    int, 
                                    int, int, double
                                >;
    REQUIRE(std::is_same_v<TL::flatten<test_list_4>::type, expected_list_4>);

    using test_list_5 = TL::type_list<
                                    TL::empty_list,
                                    TL::type_list<TL::type_list<TL::empty_list, TL::empty_list>, TL::type_list<TL::empty_list, double, TL::type_list<TL::empty_list>>>
                                >;
    using expected_list_5 = TL::type_list<
                                    double
                                >;
    REQUIRE(std::is_same_v<TL::flatten<test_list_5>::type, expected_list_5>);

    using test_list_6 = TL::type_list<
                                    TL::empty_list,
                                    TL::type_list<TL::type_list<TL::empty_list, TL::empty_list>, TL::type_list<TL::empty_list, TL::empty_list, TL::type_list<TL::empty_list>>>
                                >;
    using expected_list_6 = TL::type_list<>;
    REQUIRE(std::is_same_v<TL::flatten<test_list_6>::type, expected_list_6>);
}


TEST_CASE("type_list erase test") {
    using test_list_1 = TL::type_list<int, double, std::string, int, std::string>;
    using expected_list_1 = TL::type_list<int, double, int, std::string>;
    REQUIRE(std::is_same_v<TL::erase<std::string, test_list_1>::type, expected_list_1>);

    using test_list_2 = TL::type_list<int, double, std::string, std::string>;
    using expected_list_2 = TL::type_list<int, double, std::string>;
    REQUIRE(std::is_same_v<TL::erase<std::string, test_list_1>::type, expected_list_1>);

    using test_list_3 = TL::type_list<int, double, std::string, TL::type_list<TL::type_list<int>>>;
    using expected_list_3 = TL::type_list<int, double, std::string>;
    REQUIRE(std::is_same_v<TL::erase<TL::type_list<TL::type_list<int>>, test_list_3>::type, expected_list_3>);

    using test_list_4 = TL::type_list<int, double, std::string, TL::type_list<TL::type_list<int>>>;
    using expected_list_4 = TL::type_list<int, double, std::string, TL::type_list<TL::type_list<int>>>;
    REQUIRE(std::is_same_v<TL::erase<float, test_list_4>::type, expected_list_4>);
}


TEST_CASE("type_list erase_all test") {
    using test_list_1 = TL::type_list<int, double, int, std::string, std::string>;
    using expected_list_1 = TL::type_list<double, std::string, std::string>;
    REQUIRE(std::is_same_v<TL::erase_all<int, test_list_1>::type, expected_list_1>);

    using test_list_2 = TL::type_list<std::string, int, double, TL::type_list<std::string>>;
    using expected_list_2 = TL::type_list<int, double, TL::type_list<std::string>>;
    REQUIRE(std::is_same_v<TL::erase_all<std::string, test_list_2>::type, expected_list_2>);

    using test_list_3 = TL::type_list<>;
    using expected_list_3 = TL::type_list<>;
    REQUIRE(std::is_same_v<TL::erase_all<int, test_list_3>::type, expected_list_3>);
}


TEST_CASE("type_list erase_duplicate test") {
    using test_list_1 = TL::type_list<int, double, int, TL::empty_list, std::string, std::string, TL::empty_list>;
    using expected_list_1 = TL::type_list<int, double, TL::empty_list, std::string>;
    REQUIRE(std::is_same_v<TL::erase_duplicate<test_list_1>::type, expected_list_1>);
}

TEST_CASE("type_list replace test") {
    using test_list_1 = TL::type_list<int, double, int, std::string>;
    using expected_list_1 = TL::type_list<float, double, int, std::string>;
    REQUIRE(std::is_same_v<TL::replace<int, float, test_list_1>::type, expected_list_1>);

    using test_list_2 = TL::type_list<int>;
    using expected_list_2 = TL::type_list<std::string>;
    REQUIRE(std::is_same_v< TL::replace<int, std::string, test_list_2>::type, expected_list_2>);

    using test_list_3 = TL::type_list<std::string, int, float, TL::type_list<TL::empty_list>>;
    using expected_list_3 = TL::type_list<std::string, TL::type_list<int>, float, TL::type_list<TL::empty_list>>;
    REQUIRE(std::is_same_v<TL::replace<int, TL::type_list<int>, test_list_3>::type, expected_list_3>);

    using test_list_4 = TL::empty_list;
    using expected_list_4 = TL::empty_list;
    REQUIRE(std::is_same_v<TL::replace<int, float, test_list_4>::type, expected_list_4>);

    using test_list_6 = TL::type_list<int, TL::empty_list, float, std::string, float, TL::empty_list>;
    using expected_list_6 = TL::type_list<int, std::vector<int>, float, std::string, float, TL::empty_list>;
    REQUIRE(std::is_same_v<TL::replace<TL::empty_list, std::vector<int>, test_list_6>::type, expected_list_6>);

    using test_list_7 = TL::type_list<int, TL::empty_list, float, std::string, float, TL::empty_list>;
    using expected_list_7 = TL::type_list<int, TL::empty_list, std::string, std::string, float, TL::empty_list>;
    REQUIRE(std::is_same_v<TL::replace<float, std::string, test_list_7>::type, expected_list_7>);
}


TEST_CASE("type_list replace_all test") {
    using test_list_1 = TL::type_list<int, double, int, std::string>;
    using expected_list_1 = TL::type_list<float, double, float, std::string>;
    REQUIRE(std::is_same_v<TL::replace_all<int, float, test_list_1>::type, expected_list_1>);

    using test_list_2 = TL::type_list<int>;
    using expected_list_2 = TL::type_list<std::string>;
    REQUIRE(std::is_same_v<TL::replace_all<int, std::string, test_list_2>::type, expected_list_2>);

    using test_list_3 = TL::type_list<std::string, int, float, TL::type_list<TL::empty_list>>;
    using expected_list_3 = TL::type_list<std::string, TL::type_list<int>, float, TL::type_list<TL::empty_list>>;
    REQUIRE(std::is_same_v<TL::replace_all<int, TL::type_list<int>, test_list_3>::type, expected_list_3>);

    using test_list_4 = TL::empty_list;
    using expected_list_4 = TL::empty_list;
    REQUIRE(std::is_same_v<TL::replace_all<int, float, test_list_4>::type, expected_list_4>);

    using test_list_5 = TL::empty_list;
    using expected_list_5 = TL::empty_list;
    REQUIRE(std::is_same_v<TL::replace_all<int, float, test_list_5>::type, expected_list_5>);

    using test_list_6 = TL::type_list<int, TL::empty_list, float, std::string, float, TL::empty_list>;
    using expected_list_6 = TL::type_list<int, std::vector<int>, float, std::string, float, std::vector<int>>;
    REQUIRE(std::is_same_v<TL::replace_all<TL::empty_list, std::vector<int>, test_list_6>::type, expected_list_6>);

    using test_list_7 = TL::type_list<TL::empty_list, int, TL::empty_list, float, std::string, float, TL::empty_list, TL::empty_list>;
    using expected_list_7 = TL::type_list<TL::empty_list, int, TL::empty_list, std::string, std::string, std::string, TL::empty_list, TL::empty_list>;
    REQUIRE(std::is_same_v<TL::replace_all<float, std::string, test_list_7>::type, expected_list_7>);
}


TEST_CASE("type_list most_derived test") {
    struct Widget{};

    struct ScrollBar : public Widget {};

    struct Button : public Widget {};
    struct GraphicButton : public Button{};

    using test_list_1 = TL::type_list<GraphicButton, Widget, ScrollBar, Button>;
    REQUIRE(std::is_same_v<TL::most_derived<Widget, test_list_1>::type, GraphicButton>);

    using test_list_2 = TL::type_list<Button, ScrollBar, Widget>;
    REQUIRE(std::is_same_v<TL::most_derived<Widget, test_list_2>::type, ScrollBar>);

    using test_list_3 = TL::type_list<GraphicButton, Widget,  Button, ScrollBar>;
    REQUIRE(std::is_same_v<TL::most_derived<Widget, test_list_3>::type, ScrollBar>);
}


TEST_CASE("type_list partially_order test") {
    struct Widget{};

    struct ScrollBar : public Widget {};

    struct Button : public Widget {};
    struct GraphicButton : public Button{};

    using test_list_1 = TL::type_list<Widget, ScrollBar, Button, GraphicButton>;
    using expected_list_1 = TL::type_list<GraphicButton, ScrollBar, Button, Widget>;
    REQUIRE(std::is_same_v<TL::derived_to_front<test_list_1>::type, expected_list_1>);
}