
#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#define CATCH2_TEST_BUILD 1

#include <algorithm>
#include <array>
#include <cctype>
#include <easyjson.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string_view>

namespace fs = std::filesystem;

struct JSON_Fixture
{
    const std::string object_string = "{\n  \"a\" : {\n    \"b\" : \"c\"\n  "
                                      "},\n  \"b\": 1,\n  \"c\": false\n}";
    const std::string null_string = "null";
    const std::string array_string = "[\"a\",\"b\",\"c\",{\"d\": \"e\"}]";
    const std::string string_string = "Hello World";
    const double float_string = 0.00001;
    const bool bool_string = true;
    const long int_string = 5055559593;

    JSON_Fixture()
        : object_test(easyjson::JSON::load(object_string))
        , array_test(easyjson::JSON::load(array_string))
    {
    }

    easyjson::JSON object_test;
    easyjson::JSON null_test;
    easyjson::JSON array_test;
    easyjson::JSON string_test{ string_string };
    easyjson::JSON float_test{ float_string };
    easyjson::JSON bool_test{ bool_string };
    easyjson::JSON int_test{ int_string };
};

inline std::string trim_string(std::string const& str)
{
    std::string new_string;
    std::copy_if(str.begin(),
        str.end(),
        std::back_inserter(new_string),
        [](unsigned char c) { return !std::isspace(c); });

    return new_string;
}

TEST_CASE("Run all example cases without fail")
{
    fs::path current_directory = std::filesystem::current_path();

    const auto cases_path = fs::path(current_directory /= "test/cases");
    for (const auto& entry : fs::directory_iterator(cases_path)) {
        if (entry.is_regular_file()) {

            std::ifstream f(entry.path(), std::ios::in | std::ios::binary);
            const auto sz = fs::file_size(entry);
            std::string result(sz, '\0');
            f.read(result.data(), sz);

            REQUIRE_NOTHROW(easyjson::JSON::load(result));

            f.close();
        }
    }
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::constructors")
{
    easyjson::JSON test{ object_test };
    easyjson::JSON test_2(false);
    easyjson::JSON null;
    easyjson::JSON test_3("hello");
    easyjson::JSON test_4(static_cast<double>(0.000005));
    easyjson::JSON test_5(555);

    REQUIRE(test.JSON_type() == easyjson::JSON::Class::Object);
    REQUIRE(null.JSON_type() == easyjson::JSON::Class::Null);
    REQUIRE(test_2.JSON_type() == easyjson::JSON::Class::Boolean);
    REQUIRE(test_3.JSON_type() == easyjson::JSON::Class::String);
    REQUIRE(test_4.JSON_type() == easyjson::JSON::Class::Floating);
    REQUIRE(test_5.JSON_type() == easyjson::JSON::Class::Integral);
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::operator==")
{
    easyjson::JSON test{ object_test };
    easyjson::JSON test2{ array_test };
    easyjson::JSON test3 = easyjson::JSON::load(object_string);
    easyjson::JSON test4 = easyjson::array();
    const std::string array_string_test = "[\"a\",\"b\",\"c\"]";
    test4[0] = easyjson::JSON("a");
    test4[1] = easyjson::JSON("b");
    test4[2] = easyjson::JSON("c");
    REQUIRE(easyjson::object == easyjson::object);
    REQUIRE(easyjson::JSON("test") == easyjson::JSON("test"));
    REQUIRE(easyjson::JSON(1) == easyjson::JSON(1));
    REQUIRE(test4 == easyjson::JSON::load(array_string_test));
    REQUIRE(test2 != object_test);
#ifdef __clang__
    REQUIRE(test2 == array_test);
    REQUIRE(test == object_test);
#endif
    REQUIRE(test3 == object_test);
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::array")
{
    easyjson::JSON test = easyjson::array();
    test[0] = easyjson::JSON("a");
    test[1] = easyjson::JSON("b");
    test[2] = easyjson::JSON("c");
    test[3] = easyjson::JSON::load("{\"d\" : \"e\"}");

    REQUIRE(trim_string(test) == trim_string(std::string{ array_test }));
    auto test_array = test.dump();
    REQUIRE(trim_string(test_array) == trim_string(array_string));

    test = easyjson::array(true, false, 1);

    REQUIRE(test.JSON_type() == easyjson::JSON::Class::Array);
    REQUIRE(trim_string(test) == "[true,false,1]");
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::object")
{
    easyjson::JSON test = easyjson::object();
    test["a"] = easyjson::object();
    test["a"]["b"] = easyjson::object();
    test["a"]["b"]["c"] = easyjson::JSON("d");
    test["b"] = easyjson::JSON(1);
    test["c"] = easyjson::JSON(false);
    auto test_object = test.dump();
    auto expected = "{\n  \"a\" : {\n    \"b\" : {\n      \"c\" : \"d\"\n    "
                    "}\n  },\n  \"b\" : 1,\n  \"c\" : false\n}";
    REQUIRE(trim_string(test_object) == trim_string(expected));
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::load")
{
    easyjson::JSON obj = easyjson::JSON::load(object_string);

    REQUIRE(trim_string(obj) == trim_string(object_string));
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::load_file")
{
    easyjson::JSON test_1 = easyjson::JSON::load_file("./test/cases/test.json");
    easyjson::JSON test_2 =
        easyjson::JSON::load_file("./test/cases/test9.json");

    std::string expected =
        R"qaud({
  "product" : {
    "availability" : {
      "in_stock" : true,
      "last_restock" : "2025-08-20T10:30:00Z",
      "quantity" : 150
    },
    "category" : "Electronics",
    "name" : "Wireless Noise-Cancelling Headphones",
    "price" : 249.990000,
    "product_id" : "P001",
    "related_products" : ["P002", "P003"],
    "reviews" : [{
        "comment" : "Excellent sound quality and very comfortable for long\n  use.",
        "date" : "2025-09-01T14:15:00Z",
        "rating" : 5,
        "review_id" : "R001",
        "user_id" : "U101"
      }, {
        "comment" : "Noise cancellation works great, but charging cable is a\n  bit short.",
        "date" : "2025-08-28T09:00:00Z",
        "rating" : 4,
        "review_id" : "R002",
        "user_id" : "U102"
      }],
    "specifications" : {
      "battery_life_hours" : 30,
      "color" : ["Black", "Silver", "Midnight Blue"],
      "connectivity" : ["Bluetooth 5.2", "Auxiliary"],
      "features" : ["Active Noise Cancellation", "Transparency Mode", "VoiceAssistantIntegration"]
    }
  }
})qaud";
    auto expected_2 = "[1,2,3]";

    REQUIRE(std::string{ test_1 } == expected);
    REQUIRE(trim_string(test_2) == expected_2);
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::make")
{
    easyjson::JSON obj = easyjson::JSON::make(easyjson::JSON::Class::Boolean);

    REQUIRE(obj.JSON_type() == easyjson::JSON::Class::Boolean);
    REQUIRE(std::string{ obj } == "false");
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::to_map")
{
    auto test = object_test.to_map();
    REQUIRE(test.at("a").JSON_type() == easyjson::JSON::Class::Object);
    std::string expected = R"qaud({"b":"c"})qaud";
    REQUIRE(trim_string(std::string{ test.at("a") }) == expected);
    REQUIRE(test["b"] == easyjson::JSON(1));
    REQUIRE(test["c"] == easyjson::JSON(false));
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::object_range")
{
    auto expected = std::array<std::string, 3>{ "a", "b", "c" };
    size_t index = 0;
    for (auto const& test : object_test.object_range()) {
        REQUIRE(test.first == expected[index]);
        if (index == 0UL) {
            REQUIRE(test.second ==
                    easyjson::JSON::load(" {\n    \"b\" : \"c\"\n  }"));
        }
        index++;
    }
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::to_deque")
{
    auto test = array_test.to_deque();
    std::string expected = R"qaud({"d":"e"})qaud";
    REQUIRE(test[0] == easyjson::JSON{ "a" });
    REQUIRE(test[1] == easyjson::JSON{ "b" });
    REQUIRE(test[2] == easyjson::JSON{ "c" });
    REQUIRE(test[3] == easyjson::JSON::load(expected));
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::array_range")
{
    auto expected = std::array<std::string, 4>{
        "\"a\"", "\"b\"", "\"c\"", R"qaud({"d":"e"})qaud"
    };
    size_t index = 0;
    for (auto const& test : array_test.array_range()) {
        REQUIRE(trim_string(std::string{ test }) == expected[index]);
        index++;
    }
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::is_null")
{
    REQUIRE(null_test.is_null() == true);
    REQUIRE(easyjson::JSON{}.is_null() == true);
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::to_string")
{
    REQUIRE(string_test.to_string() == "Hello World");
    REQUIRE(int_test.to_string() == "");
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::to_float")
{
    REQUIRE(float_test.to_float() == float_string);
    REQUIRE(string_test.to_float() == 0.0);
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::to_int")
{
    REQUIRE(int_test.to_int() == int_string);
    REQUIRE(string_test.to_int() == 0);
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::to_bool")
{
    REQUIRE(bool_test.to_bool() == true);
    REQUIRE(string_test.to_bool() == false);
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::JSON_type")
{
    using namespace easyjson;
    REQUIRE(object_test.JSON_type() == JSON::Class::Object);
    REQUIRE(array_test.JSON_type() == JSON::Class::Array);
    REQUIRE(bool_test.JSON_type() == JSON::Class::Boolean);
    REQUIRE(int_test.JSON_type() == JSON::Class::Integral);
    REQUIRE(float_test.JSON_type() == JSON::Class::Floating);
    REQUIRE(null_test.JSON_type() == JSON::Class::Null);
}

TEST_CASE_METHOD(JSON_Fixture, "exception")
{
    using namespace easyjson;

    REQUIRE_THROWS(JSON::load("bad json"));
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::length")
{
    REQUIRE(array_test.length() == 4);
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::size")
{
    REQUIRE(array_test.size() == 4);
    REQUIRE(object_test.size() == 3);
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::has_key")
{
    REQUIRE(object_test.has_key("a") == true);
    REQUIRE(object_test.has_key("b") == true);
    REQUIRE(object_test.has_key("c") == true);
    REQUIRE(object_test.has_key("d") == false);
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::append")
{
    auto test = easyjson::JSON::load(array_string);
    test.append("abc");
    test.append(123);
    REQUIRE(test.size() == 6);
    REQUIRE(test[test.size() - 1] == easyjson::JSON{ 123 });
    REQUIRE(test[test.size() - 2] == easyjson::JSON{ "abc" });
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::dump")
{
    REQUIRE(trim_string(object_test.dump()) == trim_string(object_string));
    REQUIRE(trim_string(array_test.dump()) == trim_string(array_string));
}

TEST_CASE_METHOD(JSON_Fixture, "JSON::dump_keys")
{
    auto expected = std::array<std::string, 3>{ "a", "b", "c" };
    size_t index = 0;
    for (auto const& key : object_test.dump_keys()) {
        REQUIRE(key == expected[index]);
        index++;
    }
}
