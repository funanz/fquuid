// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#include <algorithm>
#include <array>
#include <compare>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "fquuid.hpp"

using namespace fquuid;

#define runtime_assert(expr, msg) [](bool ok){ if (!ok) throw std::runtime_error(msg " " #expr); }(expr)

static void test_zero()
{
    constexpr uuid a;
    constexpr uuid b {"d604557f-6739-4883-b627-bc0a81b84e97"};

    static_assert(a.is_zero(), "test_zero() #1");
    static_assert(!b.is_zero(), "test_zero() #2");
    runtime_assert(a.to_string() == "00000000-0000-0000-0000-000000000000", "test_zero() #3");
}

static void test_compare()
{
    constexpr auto a = uuid{"01926c01-ba2c-7315-a16a-0e16d8a51d4c"};
    constexpr auto b = uuid{"01926c01-ba2c-798b-8dcc-04e45ae6188b"};
    constexpr auto c = uuid{"01926c01-ba2d-708e-90a1-cc63e523be32"};
    constexpr auto d = uuid{"01926c01-ba2c-7315-a16a-0e16d8a51d4c"};

    static_assert(a < b, "test_compare() #1");
    static_assert(a < c, "test_compare() #2");
    static_assert(a == d, "test_compare() #3");
    static_assert(b < c, "test_compare() #4");
    static_assert(b > d, "test_compare() #5");
    static_assert(c > d, "test_compare() #6");

    static_assert(a != b, "test_compare() #7");

    static_assert(a <= b, "test_compare() #8");
    static_assert(b >= a, "test_compare() #9");
    static_assert(a <= d, "test_compare() #10");
    static_assert(a >= d, "test_compare() #11");

    static_assert((a <=> b) < 0, "test_compare() #12");
    static_assert((b <=> a) > 0, "test_compare() #13");
    static_assert((a <=> d) == 0, "test_compare() #14");
}

static void test_random()
{
    uuid_generator_v4 v4;
    uuid_generator_v7 v7;
    uuid_random rng;

    auto a = v4();
    auto b = v7();
    auto c = uuid_generator_v4::generate(rng);
    auto d = uuid_generator_v7::generate(rng);

    runtime_assert(a.get_version() == 4, "test_random() #1");
    runtime_assert(b.get_version() == 7, "test_random() #2");
    runtime_assert(c.get_version() == 4, "test_random() #3");
    runtime_assert(d.get_version() == 7, "test_random() #4");

    runtime_assert(a != b, "test_random() #5");
    runtime_assert(a != c, "test_random() #6");
    runtime_assert(a != d, "test_random() #7");
    runtime_assert(b != c, "test_random() #8");
    runtime_assert(b != d, "test_random() #9");
    runtime_assert(c != d, "test_random() #10");
}

static void test_time()
{
    uuid_random rng;

    auto a = uuid_generator_v7::generate(rng, 0x01926c01'ba2c);
    auto b = uuid_generator_v7::generate(rng, 0x01926c01'ba2c);

    runtime_assert(a.to_string().substr(0, 13) == "01926c01-ba2c", "test_time() #1");
    runtime_assert(b.to_string().substr(0, 13) == "01926c01-ba2c", "test_time() #2");
    runtime_assert(a != b, "test_time() #3");
}

static void test_parse()
{
    constexpr auto a = uuid{"d604557f-6739-4883-b627-bc0a81b84e97"};
    constexpr auto b = uuid{"d604557f67394883b627bc0a81b84e97"};
    constexpr auto c = uuid{"d604557f67394883b627bc0a81b84e97\n"};
    constexpr auto d = uuid{"d604557f-6739-4883-b627-bc0a81b84e97000"};

    constexpr auto span = std::span("d604557f-6739-4883-b627-bc0a81b84e97");
    constexpr auto e = uuid{span};

    std::string s = "d604557f-6739-4883-b627-bc0a81b84e97";
    auto f = uuid{s};

    static_assert(a == b, "test_parse() #1");
    static_assert(b == c, "test_parse() #2");
    static_assert(c == d, "test_parse() #3");
    static_assert(d == e, "test_parse() #4");
    runtime_assert(e == f, "test_parse() #5");
}

static void test_parse_error()
{
    try {
        const char* p = nullptr;
        uuid{p};
        runtime_assert(0, "test_parse_error() #1");
    }
    catch (std::invalid_argument&) {}

    try {
        uuid{""};
        runtime_assert(0, "test_parse_error() #2");
    }
    catch (std::invalid_argument&) {}

    try {
        uuid{"d604557f-6739-4883-b627-bc0a81b84e9"};
        runtime_assert(0, "test_parse_error() #3");
    }
    catch (std::invalid_argument&) {}

    try {
        uuid{"d604557f-6739-4883-b627b-c0a81b84e97"};
        runtime_assert(0, "test_parse_error() #4");
    }
    catch (std::invalid_argument&) {}

    try {
        uuid{"d604557f67394883b627bc0a81b84e9"};
        runtime_assert(0, "test_parse_error() #5");
    }
    catch (std::invalid_argument&) {}

    try {
        uuid{"d604557f-6739-x883-b627b-c0a81b84e97"};
        runtime_assert(0, "test_parse_error() #6");
    }
    catch (std::invalid_argument&) {}
}

static void test_bytes()
{
    constexpr auto a = uuid{"d604557f-6739-4883-b627-bc0a81b84e97"};

    constexpr std::array<uint8_t, 16> bytes_a {
        0xd6,0x04,0x55,0x7f,0x67,0x39,0x48,0x83,
        0xb6,0x27,0xbc,0x0a,0x81,0xb8,0x4e,0x97,
    };

    constexpr auto b = uuid{bytes_a};

    constexpr auto bytes_b = [&] {
        std::array<uint8_t, 16> bytes;
        a.to_bytes(bytes);
        return bytes;
    }();

    static_assert(a == b, "test_bytes() #1");
    static_assert(bytes_a == bytes_b, "test_bytes() #2");
}

static void test_bytes_error()
{
    try {
        constexpr auto a = uuid{"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<uint8_t, 15> bytes;
        a.to_bytes(bytes);
        runtime_assert(0, "test_bytes_error() #1");
    }
    catch (std::invalid_argument&) {}

    try {
        constexpr std::array<uint8_t, 15> bytes {
            0xd6,0x04,0x55,0x7f,0x67,0x39,0x48,0x83,
            0xb6,0x27,0xbc,0x0a,0x81,0xb8,0x4e,
        };

        uuid{bytes};
        runtime_assert(0, "test_bytes_error() #2");
    }
    catch (std::invalid_argument&) {}
}

static void test_string()
{
    constexpr auto a = uuid{"d604557f67394883b627bc0a81b84e97"};

    auto s1 = a.to_string();

    constexpr auto s2_array = [&] {
        std::array<char, 40> buf;
        std::ranges::fill(buf, '*');
        a.to_string_z(buf);
        return buf;
    }();
    std::string s2(s2_array.data());

    constexpr auto s3_array = [&] {
        std::array<char, 40> buf;
        std::ranges::fill(buf, '*');
        a.to_string(buf);
        return buf;
    }();
    std::string s3(s3_array.data(), s3_array.size());

    runtime_assert(s1 == "d604557f-6739-4883-b627-bc0a81b84e97", "test_string() #1");
    runtime_assert(s2 == "d604557f-6739-4883-b627-bc0a81b84e97", "test_string() #2");
    runtime_assert(s3 == "d604557f-6739-4883-b627-bc0a81b84e97****", "test_string() #3");
    static_assert(s2_array.size() == 40, "test_string() #4");
    static_assert(s3_array.size() == 40, "test_string() #5");
}

static void test_string_error()
{
    try {
        constexpr auto a = uuid{"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<char, 35> buf;
        a.to_string(buf);
        runtime_assert(0, "test_string_error() #1");
    }
    catch (std::invalid_argument&) {}

    try {
        constexpr auto a = uuid{"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<char, 36> buf;
        a.to_string_z(buf);
        runtime_assert(0, "test_string_error() #2");
    }
    catch (std::invalid_argument&) {}
}

static void test_ostream()
{
    constexpr auto a = uuid{"d604557f67394883b627bc0a81b84e97"};

    std::ostringstream oss;
    oss << "{" << a << "}";

    runtime_assert(oss.str() == "{d604557f-6739-4883-b627-bc0a81b84e97}", "test_ostream() #1");
}

template <class Map>
static void test_map_impl()
{
    constexpr auto a = uuid{"01926c01-ba2c-7315-a16a-0e16d8a51d4c"};
    constexpr auto b = uuid{"01926c01-ba2c-798b-8dcc-04e45ae6188b"};
    constexpr auto c = uuid{"01926c01-ba2d-708e-90a1-cc63e523be32"};

    Map m {
        { uuid{"01926c01-ba2c-7315-a16a-0e16d8a51d4c"}, "a" },
        { uuid{"01926c01-ba2c-798b-8dcc-04e45ae6188b"}, "b" },
        { uuid{"01926c01-ba2d-708e-90a1-cc63e523be32"}, "c" },
    };

    runtime_assert(m.size() == 3, "test_map() #1");
    runtime_assert(m.at(a) == "a", "test_map() #2");
    runtime_assert(m.at(b) == "b", "test_map() #3");
    runtime_assert(m.at(c) == "c", "test_map() #4");

    auto [it, insert] = m.insert_or_assign(a, "x");
    runtime_assert(!insert, "test_map() #5");
    runtime_assert(m.size() == 3, "test_map() #6");
    runtime_assert(m.at(a) == "x", "test_map() #7");
    runtime_assert(m.at(b) == "b", "test_map() #8");
    runtime_assert(m.at(c) == "c", "test_map() #9");
}

static void test_map()
{
    test_map_impl<std::map<uuid, std::string>>();
    test_map_impl<std::unordered_map<uuid, std::string>>();
}

int main(int argc, char** argv)
{
    try {
        test_zero();
        test_compare();
        test_random();
        test_time();
        test_parse();
        test_parse_error();
        test_bytes();
        test_bytes_error();
        test_string();
        test_string_error();
        test_ostream();
        test_map();

        std::cout << "All tests successful" << std::endl;
        return 0;
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
