#include "lib/catch.hpp"
#include "src/any.hpp"

#include <string>
#include <vector>
#include <memory>

TEST_CASE("any constructor with heap initialization tests") {
    // empty
    any empty_any;
    REQUIRE(empty_any.has_value() == false);
    REQUIRE(empty_any.type() == typeid(void));

    // rvalue ref
    any rvalue_ref_any(std::string("this is a string"));
    REQUIRE(rvalue_ref_any.has_value() == true);
    REQUIRE(rvalue_ref_any.type() == typeid(std::string));

    // lvalue
    std::string lvalue = "this is a lvalue string";
    any lvalue_any(lvalue);
    REQUIRE(lvalue_any.has_value() == true);
    REQUIRE(lvalue_any.type() == typeid(std::string));

    // lvalue ref
    std::string &lvalue_ref = lvalue; 
    any lvalue_ref_any(lvalue_ref);
    REQUIRE(lvalue_ref_any.has_value() == true);
    REQUIRE(lvalue_ref_any.type() == typeid(std::string));

    // const lvalue ref
    const std::string &const_lvalue_ref = lvalue;
    any const_lvalue_ref_any(const_lvalue_ref);
    REQUIRE(const_lvalue_ref_any.has_value() == true);
    REQUIRE(const_lvalue_ref_any.type() == typeid(std::string));
}


TEST_CASE("const T* any_cast(const any*) with heap initialization tests") {
    struct test {
        std::string str;
        int i;
        double d;
    };

    // rvalue
    const any rvalue_any(test{"this is a test rvalue ref string", 14, 5.14});
    const test *rvalue_cast = any_cast<test>(&rvalue_any); 
    REQUIRE(rvalue_cast->str == "this is a test rvalue ref string");
    REQUIRE(rvalue_cast->i == 14);
    REQUIRE(rvalue_cast->d == Approx(5.14));

    // lvalue
    test lvalue_obj{"this is a test string", 23, 3.14};
    const any lvalue_any(lvalue_obj);
    const test *lvalue_cast = any_cast<test>(&lvalue_any);
    REQUIRE(lvalue_cast != nullptr);
    REQUIRE(lvalue_cast->str == "this is a test string");
    REQUIRE(lvalue_cast->i == 23);
    REQUIRE(lvalue_cast->d == Approx(3.14));

    // lvalue reference
    test &lvalue_ref_obj = lvalue_obj;
    const any lvalue_ref_any(lvalue_ref_obj);
    const test *lvalue_ref_cast = any_cast<test>(&lvalue_ref_any);
    REQUIRE(lvalue_ref_cast != nullptr);
    REQUIRE(lvalue_ref_cast->str == "this is a test string");
    REQUIRE(lvalue_ref_cast->i == 23);
    REQUIRE(lvalue_ref_cast->d == Approx(3.14));

    // const lvalue reference
    const test &const_lvalue_ref_obj = lvalue_obj;
    const any const_lvalue_ref_any(const_lvalue_ref_obj);
    const test *const_lvalue_ref_cast = any_cast<test>(&const_lvalue_ref_any);
    REQUIRE(const_lvalue_ref_cast != nullptr);
    REQUIRE(const_lvalue_ref_cast->str == "this is a test string");
    REQUIRE(const_lvalue_ref_cast->i == 23);
    REQUIRE(const_lvalue_ref_cast->d == Approx(3.14));
}


TEST_CASE("T* any_cast(any*) with heap initialization tests") {
    struct test {
        char raw_str[8086];
        std::string str;
    };

    // rvalue
    any rvalue_any(std::vector<std::string>{"this is string 1", "this is string 2", "this is string 3", "this is string 4"});
    std::vector<std::string> *rvalue_cast = any_cast<std::vector<std::string>>(&rvalue_any);
    REQUIRE(rvalue_cast != nullptr);
    REQUIRE(*rvalue_cast == std::vector<std::string>{"this is string 1", "this is string 2", "this is string 3", "this is string 4"});
    rvalue_cast->push_back("this is string 5");
    rvalue_cast->push_back("this is string 6");
    REQUIRE(*rvalue_cast == std::vector<std::string>{"this is string 1", "this is string 2", "this is string 3", "this is string 4", "this is string 5", "this is string 6"});
    REQUIRE(*any_cast<std::vector<std::string>>(&rvalue_any) == std::vector<std::string>{"this is string 1", "this is string 2", "this is string 3", "this is string 4", "this is string 5", "this is string 6"});

    // lvalue
    test lvalue = {"this is a raw string", "this is stl string"};
    any lvalue_any(lvalue);
    test *lvalue_cast = any_cast<test>(&lvalue_any);
    REQUIRE(lvalue_cast != nullptr);
    REQUIRE(std::strcmp(lvalue_cast->raw_str, "this is a raw string") == 0);
    REQUIRE(lvalue_cast->str == "this is stl string");
    std::strcpy(lvalue_cast->raw_str, "this is a modified raw string");
    REQUIRE(std::strcmp(lvalue_cast->raw_str, "this is a modified raw string") == 0);

    // lvalue reference
    test &lvalue_ref = lvalue;
    any lvalue_ref_any(lvalue_ref);
    test *lvalue_ref_cast = any_cast<test>(&lvalue_ref_any);
    REQUIRE(lvalue_ref_cast != nullptr);
    REQUIRE(std::strcmp(lvalue_ref_cast->raw_str, "this is a raw string") == 0);
    REQUIRE(lvalue_ref_cast->str == "this is stl string");

    // const lvalue reference
    const test &const_lvalue_ref = lvalue;
    any const_lvalue_ref_any(const_lvalue_ref);
    test *const_lvalue_ref_cast = any_cast<test>(&const_lvalue_ref_any);
    REQUIRE(const_lvalue_ref_cast != nullptr);
    REQUIRE(std::strcmp(const_lvalue_ref_cast->raw_str, "this is a raw string") == 0);
    REQUIRE(const_lvalue_ref_cast->str == "this is stl string");
}


TEST_CASE("T any_cast(any&) with heap initialization tests") {
    any ref_any(std::string{"this is a test string"});

    std::string value_cast = any_cast<std::string>(ref_any);
    REQUIRE(value_cast == "this is a test string");

    std::string &ref_cast = any_cast<std::string &>(ref_any);
    REQUIRE(ref_cast == "this is a test string");
    ref_cast += " with concat string";
    REQUIRE(any_cast<std::string &>(ref_any) == "this is a test string with concat string");

    const std::string &const_ref_cast = any_cast<const std::string&>(ref_any);
    REQUIRE(const_ref_cast == "this is a test string with concat string");

    REQUIRE_THROWS_AS(any_cast<int>(ref_any), bad_any_cast);
    REQUIRE_THROWS_AS(any_cast<std::string *>(ref_any), bad_any_cast);
}


TEST_CASE("T any_cast(const any&) with heap initialization tests") {
    const any const_any(std::string{"this is a test string"});

    std::string value_cast = any_cast<std::string>(const_any);
    REQUIRE(value_cast == "this is a test string");

    const std::string &const_lvalue_ref_value = any_cast<const std::string &>(const_any);
    REQUIRE(const_lvalue_ref_value == "this is a test string");
}


TEST_CASE("T any_cast(any &&) with heap initialization tests") {
    any move_any(std::string{"this is a test string"});

    std::string value_cast = any_cast<std::string>(std::move(move_any));
    REQUIRE(value_cast == "this is a test string");
}


TEST_CASE("any constructor with stack initialization") {
    struct test {
        std::string str;
        double d;
        int i;
    };

    // integer
    any int_any(12);
    int int_cast = any_cast<int>(int_any);
    REQUIRE(int_cast == 12);

    any cp_int_any(int_any);
    int_cast = any_cast<int>(cp_int_any);
    REQUIRE(int_cast == 12);

    any mv_int_any(std::move(int_any));
    int_cast = any_cast<int>(mv_int_any);
    REQUIRE(int_cast == 12);

    // double
    any double_any(3.14);
    double double_cast = any_cast<double>(double_any);
    REQUIRE(double_cast == Approx(3.14));

    any cp_double_any(double_any);
    double_cast = any_cast<double>(cp_double_any);
    REQUIRE(double_cast == Approx(3.14));

    any mv_double_any(std::move(double_any));
    double_cast = any_cast<double>(mv_double_any);
    REQUIRE(double_cast == Approx(3.14));

    // raw string
    any char_any("this is a raw string");
    const char *raw_str_cast = any_cast<const char*>(char_any);
    REQUIRE(std::strcmp(raw_str_cast, "this is a raw string"));

    // smart ptr
    std::shared_ptr<test> smrt_ptr = std::make_shared<test>(); 
    smrt_ptr->str = "this is a test string";
    smrt_ptr->d = 3.14;
    smrt_ptr->i = 12;
    REQUIRE(smrt_ptr.use_count() == 1);
    any smrt_ptr_any(smrt_ptr);
    std::shared_ptr<test> &smrt_ptr_cast = any_cast<std::shared_ptr<test>&>(smrt_ptr_any);
    REQUIRE(smrt_ptr_cast.use_count() == 2);
    REQUIRE(smrt_ptr_cast->str == "this is a test string");
    REQUIRE(smrt_ptr_cast->i == 12);
    REQUIRE(smrt_ptr_cast->d == Approx(3.14));
    REQUIRE(smrt_ptr.use_count() == 2);

    any cp_smrt_ptr_any(smrt_ptr_any);
    std::shared_ptr<test> &cp_smrt_ptr_cast = any_cast<std::shared_ptr<test>&>(cp_smrt_ptr_any);
    REQUIRE(cp_smrt_ptr_cast.use_count() == 3);
    REQUIRE(smrt_ptr_cast.use_count() == 3);
    REQUIRE(smrt_ptr.use_count() == 3);

    any mv_smrt_ptr_any(std::move(cp_smrt_ptr_any));
    std::shared_ptr<test> &mv_smrt_ptr_cast = any_cast<std::shared_ptr<test>&>(mv_smrt_ptr_any);
    REQUIRE(mv_smrt_ptr_cast.use_count() == 3);
    REQUIRE(cp_smrt_ptr_cast == nullptr);
    REQUIRE(smrt_ptr_cast.use_count() == 3);
    REQUIRE(smrt_ptr.use_count() == 3);
}


TEST_CASE("any operator= tests") {
    struct test {
        test(std::string str, int i, double d) {
            this->str = str;
            this->i = i;
            this->d = d;
        };

        std::string str;
        int i;
        double d;
    };

    // integer
    any int_any = 12;
    int int_cast = any_cast<int>(int_any);
    REQUIRE(int_cast == 12);

    // double
    any double_any = 3.14;
    double double_cast = any_cast<double>(double_any);
    REQUIRE(double_cast == Approx(3.14));

    // string
    any str_any = std::string("this is a string");
    std::string str_cast = any_cast<std::string>(str_any);
    REQUIRE(str_cast == "this is a string");

    any cp_str_any = str_any;
    str_cast = any_cast<std::string>(cp_str_any);
    REQUIRE(str_cast == "this is a string");

    any mv_str_any = std::move(cp_str_any);
    str_cast = any_cast<std::string>(mv_str_any);
    REQUIRE(str_cast == "this is a string");

    // vector
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8};
    any vec_any = vec;
    std::vector<int> vec_cast = any_cast<std::vector<int>>(vec_any);
    REQUIRE(vec_cast == vec);

    // smart ptr
    any smrt_ptr_any = std::make_shared<test>("this is a test string", 212, 3.1444);
    std::shared_ptr<test> &test_cast = any_cast<std::shared_ptr<test> &>(smrt_ptr_any);
    REQUIRE(test_cast.use_count() == 1);
    REQUIRE(test_cast->str == "this is a test string");
    REQUIRE(test_cast->d == Approx(3.1444));
    REQUIRE(test_cast->i == 212);
    test_cast->i = 3;
    REQUIRE(any_cast<std::shared_ptr<test> &>(smrt_ptr_any)->i == 3);

    any cp_smrt_ptr_any = smrt_ptr_any;
    std::shared_ptr<test> &cp_test_cast = any_cast<std::shared_ptr<test> &>(cp_smrt_ptr_any);
    REQUIRE(cp_test_cast.use_count() == 2);
    REQUIRE(cp_test_cast->str == "this is a test string");
    REQUIRE(cp_test_cast->d == Approx(3.1444));
    REQUIRE(cp_test_cast->i == 3);

    any mv_smrt_ptr_any = std::move(smrt_ptr_any);
    std::shared_ptr<test> &mv_test_cast = any_cast<std::shared_ptr<test> &>(mv_smrt_ptr_any);
    REQUIRE(mv_test_cast.use_count() == 2);
    REQUIRE(test_cast == nullptr);
}


TEST_CASE("any swap method") {
    // swap integer and double
    any int_any = 1;
    REQUIRE(any_cast<int>(int_any) == 1);

    any double_any = 3.14;
    REQUIRE(any_cast<double>(double_any) == Approx(3.14));

    int_any.swap(double_any);
    REQUIRE(double_any.type() == typeid(int));
    REQUIRE(any_cast<int>(double_any) == 1);
    REQUIRE(int_any.type() == typeid(double));
    REQUIRE(any_cast<double>(int_any) == Approx(3.14));

    // swap smart ptr    
    any sharedptr_any = std::make_shared<int>(4);
    REQUIRE(any_cast<std::shared_ptr<int> &>(sharedptr_any).use_count() == 1);
    REQUIRE(*any_cast<std::shared_ptr<int> &>(sharedptr_any) == 4);

    any str_any = std::string("this is a test string");
    REQUIRE(str_any.type() == typeid(std::string));
    REQUIRE(any_cast<std::string>(str_any) == "this is a test string");

    str_any.swap(sharedptr_any);
    REQUIRE(str_any.type() == typeid(std::shared_ptr<int>));
    REQUIRE(any_cast<std::shared_ptr<int>&>(str_any).use_count() == 1);
    REQUIRE(*any_cast<std::shared_ptr<int> &>(str_any) == 4);
    REQUIRE(sharedptr_any.type() == typeid(std::string));
    REQUIRE(any_cast<std::string>(sharedptr_any) == "this is a test string");    
}


TEST_CASE("any reset method") {
    struct test {
        int i;

        ~test() {
            --i;
        }
    };

    // lvalue
    any test_any = test{5};
    test &test_cast = any_cast<test&>(test_any);
    REQUIRE(test_cast.i == 5);

    test_any.reset();
    REQUIRE(test_any.has_value() == false);
    REQUIRE(test_any.type() == typeid(void));
    REQUIRE(test_cast.i == 4);

    // raw ptr
    any raw_ptr_any = new test{5};
    test *test_raw_ptr_cast = any_cast<test*>(raw_ptr_any);
    REQUIRE(test_raw_ptr_cast->i == 5);
    raw_ptr_any.reset();
    REQUIRE(raw_ptr_any.type() == typeid(void));
    REQUIRE(raw_ptr_any.has_value() == false);
    REQUIRE(test_raw_ptr_cast->i == 5);

    // smart ptr
    std::shared_ptr<test> smrt_ptr = std::make_shared<test>();
    smrt_ptr->i = 5;
    any smrt_ptr_any = smrt_ptr;
    REQUIRE(any_cast<std::shared_ptr<test> &>(smrt_ptr_any).use_count() == 2);
    REQUIRE(smrt_ptr.use_count() == 2);
    smrt_ptr_any.reset();
    REQUIRE(smrt_ptr.use_count() == 1);
}