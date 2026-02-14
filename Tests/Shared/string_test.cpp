#include "gtest/gtest.h"

#include "fow/Shared/String.hpp"

using namespace fow;

TEST(String, Constructors) {
    const auto a = String();   // Empty string
    EXPECT_STREQ(a.as_cstr(), "");
    auto b = String { a }; // Copy constructor
    EXPECT_STREQ(b.as_cstr(), "");
    const auto c = String { std::move(b) }; // Move constructor
    EXPECT_EQ(b.as_cstr(), nullptr);
    EXPECT_STREQ(c.as_cstr(), "");

    const auto d = String { "Hello world!" }; // const char*
    EXPECT_STREQ(d.as_cstr(), "Hello world!");
    const auto e = String  { "Hello world!", 5 }; // const char* with specific length
    EXPECT_STREQ(e.as_cstr(), "Hello");
    const auto f = String { 'A', 4 }; // Specific amount of characters
    EXPECT_STREQ(f.as_cstr(), "AAAA");
    const auto g = String { std::string("Hello world!") }; // std::string
    EXPECT_STREQ(g.as_cstr(), "Hello world!");
    const auto h = String { std::string_view("Hello world!") }; // std::string_view
    EXPECT_STREQ(h.as_cstr(), "Hello world!");

    constexpr size_t dummy_size = 5;
    const char dummy_data[dummy_size] = { 0, 0, 0, 0, 0 };
    const auto i = String { dummy_size }; // Allocate with capacity
    EXPECT_EQ(i.capacity(), dummy_size);
    EXPECT_EQ(i.size(), 0);
    EXPECT_TRUE(memcmp(i.data(), dummy_data, dummy_size) == 0);
    EXPECT_TRUE(i.is_empty());

    auto j = String { nullptr }; // Using nullptr
    EXPECT_TRUE(j.is_empty());

    EXPECT_EQ(d, "Hello world!"_s); // Literal operator

    const std::vector<String> tokens = { "1", "2", "3", "4" };
    EXPECT_STREQ(String::Join(tokens, ',').as_cstr(), "1,2,3,4");
    EXPECT_STREQ(String::Join(tokens, ", ").as_cstr(), "1, 2, 3, 4");
}
TEST(String, Equals) {
    const auto a = String { "Hello world!" };
    const auto b = String { "Hello world!" };
    const auto c = String { "Hello there!" };

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    EXPECT_TRUE(a.equals_slice(c, 5));
}
TEST(String, Cloning) {
    const auto a = String { "Hello world!" };
    const auto b = String { "hello world!" };
    const auto c = String { "HELLO WORLD!" };
    EXPECT_EQ(a.clone_lowercase(), b);
    EXPECT_EQ(a.clone_uppercase(), c);

    const auto d = String { "  Test  " };
    const auto e = String { "Test  " };
    const auto f = String { "  Test" };
    const auto g = String { "Test" };
    EXPECT_EQ(d.clone_begin_trimmed(), e);
    EXPECT_EQ(d.clone_end_trimmed(), f);
    EXPECT_EQ(d.clone_trimmed(), g);

    EXPECT_STREQ(a.substr(6).as_cstr(), "world!");
    EXPECT_STREQ(a.substr(0, 5).as_cstr(), "Hello");

    const auto dummy_list = std::vector<String> {
        "1", "2", "3", "4"
    };

    const auto h = String { "1,2,3,4" };
    const auto h_list = h.split(',');
    EXPECT_TRUE(std::ranges::equal(h_list, dummy_list));

    const auto i = String { "1\n2\n3\n4" };
    const auto i_list = i.split();
    EXPECT_TRUE(std::ranges::equal(i_list, dummy_list));
}
TEST(String, EqualityCheck) {
    const auto a = String { "Hello World!" };
    const auto b = String { "hello world!" };

    EXPECT_NE(a, b);
    EXPECT_TRUE(a.equals(b, StringCompareType::CaseInsensitive));
}
TEST(String, Manipulation) {
    auto a = String { "Hello!" };
    EXPECT_STREQ(a.as_cstr(), "Hello!");
    a.pop();
    EXPECT_STREQ(a.as_cstr(), "Hello");
    a.append(' ');
    EXPECT_STREQ(a.as_cstr(), "Hello ");
    a.append("world!");
    EXPECT_STREQ(a.as_cstr(), "Hello world!");
    a.remove(6, 5);
    EXPECT_STREQ(a.as_cstr(), "Hello !");
    a.remove_to_end(5);
    EXPECT_STREQ(a.as_cstr(), "Hello");

    EXPECT_GT(a.capacity(), a.size());
    a.shrink_to_fit();
    EXPECT_EQ(a.capacity(), a.size());
    a.resize(10);
    EXPECT_EQ(a.capacity(), 10);

    const char* remains = a.data() + 5;
    EXPECT_TRUE(remains != nullptr && *remains == 0);
    a.clear();
    EXPECT_TRUE(a.is_empty());

    a = String { "HelloWorld" };
    EXPECT_STREQ(a.as_cstr(), "HelloWorld");
    a.insert(5, ' ');
    EXPECT_STREQ(a.as_cstr(), "Hello World");
    a.insert(5, ", ");
    EXPECT_STREQ(a.as_cstr(), "Hello,  World");
    a.remove(6);
    EXPECT_STREQ(a.as_cstr(), "Hello, World");
    a.replace("World", "GTest");
    EXPECT_STREQ(a.as_cstr(), "Hello, GTest");

    a = String { "AABABB" };
    a.replace("BA", "AA");
    EXPECT_STREQ(a.as_cstr(), "AAAABB");
    a.replace("AA", "BB", 2);
    EXPECT_STREQ(a.as_cstr(), "AABBBB");
    a.replace('B', 'A');
    EXPECT_STREQ(a.as_cstr(), "AAABBB");
    a.replace('B', 'A', 4);
    EXPECT_STREQ(a.as_cstr(), "AAABAB");
    a.replace_all("AB", "BA");
    EXPECT_STREQ(a.as_cstr(), "AABABA");
    a.replace_all('B', 'A');
    EXPECT_STREQ(a.as_cstr(), "AAAAAA");

    a = String { "  Test  " };
    a.trim_begin();
    EXPECT_STREQ(a.as_cstr(), "Test  ");
    a.trim_end();
    EXPECT_STREQ(a.as_cstr(), "Test");
    a = String { "  Test  " };
    a.trim();
    EXPECT_STREQ(a.as_cstr(), "Test");

    a.to_lowercase();
    EXPECT_STREQ(a.as_cstr(), "test");
    a.to_uppercase();
    EXPECT_STREQ(a.as_cstr(), "TEST");
}

TEST(String, Searching) {
    EXPECT_EQ(String::NotFound, -1);

    const auto a = String { "AABBCDEEE" };
    EXPECT_EQ(a.count('E'), 3);
    EXPECT_EQ(a.find('B'), 2);
    EXPECT_EQ(a.find('X'), String::NotFound);

    EXPECT_EQ(a.find_last('B'), 3);
    EXPECT_EQ(a.find_last('X'), String::NotFound);

    EXPECT_EQ(a.find_any_first_of("BC"), 2);
    EXPECT_EQ(a.find_any_first_of("CB"), 2);
    EXPECT_EQ(a.find_any_first_of("XYZ"), String::NotFound);

    EXPECT_EQ(a.find_any_last_of("CE"), 8);
    EXPECT_EQ(a.find_any_last_of("EC"), 8);
    EXPECT_EQ(a.find_any_last_of("XYZ"), String::NotFound);

    EXPECT_EQ(a.find_any_first_not_of("ABCD"), 6);
    EXPECT_EQ(a.find_any_first_not_of("DCBA"), 6);
    EXPECT_EQ(a.find_any_first_not_of("XYZ"), 0);
    EXPECT_EQ(a.find_any_first_not_of("ABCDE"), String::NotFound);

    EXPECT_EQ(a.find_any_last_not_of("EDC"), 3);
    EXPECT_EQ(a.find_any_last_not_of("CDE"), 3);
    EXPECT_EQ(a.find_any_last_not_of("XYZ"), 8);
    EXPECT_EQ(a.find_any_last_not_of("ABCDE"), String::NotFound);

    EXPECT_TRUE(a.starts_with('A'));
    EXPECT_TRUE(a.starts_with("AA"));
    EXPECT_TRUE(a.starts_with('a', StringCompareType::CaseInsensitive));
    EXPECT_TRUE(a.starts_with("Aa", StringCompareType::CaseInsensitive));
    EXPECT_TRUE(a.ends_with('E'));
    EXPECT_TRUE(a.ends_with("EE"));
    EXPECT_TRUE(a.ends_with('e', StringCompareType::CaseInsensitive));
    EXPECT_TRUE(a.ends_with("Ee", StringCompareType::CaseInsensitive));
}

TEST(String, Iterators) {
    auto str = String { "ABCD" };

    int i = 0;
    for (const auto& c : str) {
        EXPECT_EQ(c, 'A' + i);
        ++i;
    }
    for (auto& c : str) {
        c++;
    }
    EXPECT_STREQ(str.as_cstr(), "BCDE");
}

TEST(String, Streams) {
    std::stringstream ss;
    ss << "Test"_s;
    EXPECT_STREQ(ss.str().c_str(), "Test");

    String str;
    ss >> str;
    EXPECT_STREQ(str.as_cstr(), "Test");
}