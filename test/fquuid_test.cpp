// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#include <fquuid.hpp>
#include <algorithm>
#include <array>
#include <compare>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

using namespace fquuid;

#define CT_CHAR 0
#define CT_WCHAR_T 1632
#define CT_CHAR8_T 8
#define CT_CHAR16_T 16
#define CT_CHAR32_T 32

#if CHAR_TYPE == CT_CHAR
#   define CHAR_T char
#   define S(s) s
#elif CHAR_TYPE == CT_WCHAR_T
#   define CHAR_T wchar_t
#   define HAVE_UNICODE 1
#   define S(s) L##s
#elif CHAR_TYPE == CT_CHAR8_T
#   define CHAR_T char8_t
#   define HAVE_UNICODE 1
#   define S(s) u8##s
#elif CHAR_TYPE == CT_CHAR16_T
#   define CHAR_T char16_t
#   define HAVE_UNICODE 1
#   define S(s) u##s
#elif CHAR_TYPE == CT_CHAR32_T
#   define CHAR_T char32_t
#   define HAVE_UNICODE 1
#   define S(s) U##s
#endif

using CharT = CHAR_T;
using String = std::basic_string<CharT>;

#define runtime_assert(expr, msg) [](bool ok){ if (!ok) throw std::runtime_error(msg " " #expr); }(expr)

static void test_nil()
{
    constexpr uuid a;
    constexpr uuid b {S("d604557f-6739-4883-b627-bc0a81b84e97")};

    static_assert(a.is_nil(), "test_nil() #1");
    static_assert(!b.is_nil(), "test_nil() #2");
    runtime_assert(a.to_string() == "00000000-0000-0000-0000-000000000000", "test_nil() #3");
}

static void test_compare()
{
    constexpr auto a = uuid{S("01926c01-ba2c-7315-a16a-0e16d8a51d4c")};
    constexpr auto b = uuid{S("01926c01-ba2c-798b-8dcc-04e45ae6188b")};
    constexpr auto c = uuid{S("01926c01-ba2d-708e-90a1-cc63e523be32")};
    constexpr auto d = uuid{S("01926c01-ba2c-7315-a16a-0e16d8a51d4c")};

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
    constexpr auto a = uuid{S("d604557f-6739-4883-b627-bc0a81b84e97")};
    constexpr auto b = uuid{S("d604557f67394883b627bc0a81b84e97")};
    constexpr auto c = uuid{S("d604557f67394883b627bc0a81b84e97\n")};
    constexpr auto d = uuid{S("d604557f-6739-4883-b627-bc0a81b84e97000")};

    constexpr auto span = std::span(S("d604557f-6739-4883-b627-bc0a81b84e97"));
    constexpr auto e = uuid{span};

    constexpr const CharT* ptr = S("d604557f-6739-4883-b627-bc0a81b84e97");
    constexpr auto f = uuid{ptr};

    String s = S("d604557f-6739-4883-b627-bc0a81b84e97");
    auto g = uuid{s};

    static_assert(a == b, "test_parse() #1");
    static_assert(b == c, "test_parse() #2");
    static_assert(c == d, "test_parse() #3");
    static_assert(d == e, "test_parse() #4");
    static_assert(e == f, "test_parse() #5");
    runtime_assert(f == g, "test_parse() #6");
}

static void test_parse_error()
{
    try {
        const CharT* ptr = nullptr;
        uuid{ptr};
        runtime_assert(0, "test_parse_error() #1");
    }
    catch (std::invalid_argument&) {}

    try {
        uuid{S("")};
        runtime_assert(0, "test_parse_error() #2");
    }
    catch (std::invalid_argument&) {}

    try {
        uuid{S("d604557f-6739-4883-b627-bc0a81b84e9")};
        runtime_assert(0, "test_parse_error() #3");
    }
    catch (std::invalid_argument&) {}

    try {
        uuid{S("d604557f-6739-4883-b627b-c0a81b84e97")};
        runtime_assert(0, "test_parse_error() #4");
    }
    catch (std::invalid_argument&) {}

    try {
        uuid{S("d604557f67394883b627bc0a81b84e9")};
        runtime_assert(0, "test_parse_error() #5");
    }
    catch (std::invalid_argument&) {}

    // load_u16_hex 000x
    try {
        uuid{S("d604557f-673x-4883-b627-bc0a81b84e97")};
        runtime_assert(0, "test_parse_error() #6");
    }
    catch (std::invalid_argument&) {}

    // load_u16_hex x000
    try {
        uuid{S("d604557f-x739-4883-b627-bc0a81b84e97")};
        runtime_assert(0, "test_parse_error() #7");
    }
    catch (std::invalid_argument&) {}

    // load_u32_hex 00000000x
    try {
        uuid{S("d604557x-6739-4883-b627-bc0a81b84e97")};
        runtime_assert(0, "test_parse_error() #8");
    }
    catch (std::invalid_argument&) {}

    // load_u32_hex x00000000
    try {
        uuid{S("x604557f-6739-4883-b627-bc0a81b84e97")};
        runtime_assert(0, "test_parse_error() #9");
    }
    catch (std::invalid_argument&) {}

    // load_u48_hex 00000000000x
    try {
        uuid{S("d604557f-6739-4883-b627-bc0a81b84e9x")};
        runtime_assert(0, "test_parse_error() #10");
    }
    catch (std::invalid_argument&) {}

    // load_u48_hex x00000000000
    try {
        uuid{S("d604557f-6739-4883-b627-xc0a81b84e97")};
        runtime_assert(0, "test_parse_error() #11");
    }
    catch (std::invalid_argument&) {}
}

static void test_parse_error_unicode()
{
#if HAVE_UNICODE
    try {
        uuid{S("d604557f-6739-4883-b627-bc0a81b84e9\U00000137")};
        runtime_assert(0, "test_parse_error_unicode() #1");
    }
    catch (std::invalid_argument&) {}
#endif
}

static void test_string()
{
    constexpr auto a = uuid{S("d604557f67394883b627bc0a81b84e97")};

    auto s1 = a.to_string<String>();

    constexpr auto s2_array = [&] {
        std::array<CharT, 40> buf;
        std::ranges::fill(buf, S('*'));
        a.write_string(buf);
        return buf;
    }();
    String s2(s2_array.data());

    constexpr auto s3_array = [&] {
        std::array<CharT, 40> buf;
        std::ranges::fill(buf, S('*'));
        a.write_string(buf, string_terminator::none);
        return buf;
    }();
    String s3(s3_array.data(), s3_array.size());

    runtime_assert(s1 == S("d604557f-6739-4883-b627-bc0a81b84e97"), "test_string() #1");
    runtime_assert(s2 == S("d604557f-6739-4883-b627-bc0a81b84e97"), "test_string() #2");
    runtime_assert(s3 == S("d604557f-6739-4883-b627-bc0a81b84e97****"), "test_string() #3");
    static_assert(s2_array.size() == 40, "test_string() #4");
    static_assert(s3_array.size() == 40, "test_string() #5");
}

static void test_string_error()
{
    try {
        constexpr auto a = uuid{S("d604557f-6739-4883-b627-bc0a81b84e97")};

        std::array<CharT, 35> buf;
        a.write_string(buf, string_terminator::none);
        runtime_assert(0, "test_string_error() #1");
    }
    catch (std::invalid_argument&) {}

    try {
        constexpr auto a = uuid{S("d604557f-6739-4883-b627-bc0a81b84e97")};

        std::array<CharT, 36> buf;
        a.write_string(buf);
        runtime_assert(0, "test_string_error() #2");
    }
    catch (std::invalid_argument&) {}
}

static void test_ostream()
{
    constexpr auto a = uuid{S("d604557f67394883b627bc0a81b84e97")};

    std::basic_ostringstream<CharT> oss;
    oss << S("{") << a << S("}");

    runtime_assert(oss.str() == S("{d604557f-6739-4883-b627-bc0a81b84e97}"), "test_ostream() #1");
}

static void test_binary_u8()
{
    constexpr auto a = uuid{S("d604557f-6739-4883-b627-bc0a81b84e97")};

    constexpr std::array<uint8_t, 16> bytes_a {
        0xd6,0x04,0x55,0x7f,0x67,0x39,0x48,0x83,
        0xb6,0x27,0xbc,0x0a,0x81,0xb8,0x4e,0x97,
    };

    constexpr auto b = uuid{bytes_a};

    constexpr auto bytes_b = [&] {
        std::array<uint8_t, 16> bytes;
        a.write_bytes(bytes);
        return bytes;
    }();

    static_assert(a == b, "test_binary_u8() #1");
    static_assert(bytes_a == bytes_b, "test_binary_u8() #2");
}

static void test_binary_byte()
{
    constexpr auto a = uuid{S("d604557f-6739-4883-b627-bc0a81b84e97")};

    constexpr std::array<std::byte, 16> bytes_a {
        std::byte(0xd6),std::byte(0x04),std::byte(0x55),std::byte(0x7f),
        std::byte(0x67),std::byte(0x39),std::byte(0x48),std::byte(0x83),
        std::byte(0xb6),std::byte(0x27),std::byte(0xbc),std::byte(0x0a),
        std::byte(0x81),std::byte(0xb8),std::byte(0x4e),std::byte(0x97),
    };

    constexpr auto b = uuid{bytes_a};

    constexpr auto bytes_b = [&] {
        std::array<std::byte, 16> bytes;
        a.write_bytes(bytes);
        return bytes;
    }();

    constexpr auto bytes_u8 = [&] {
        std::array<uint8_t, 16> bytes;
        a.write_bytes(bytes);
        return bytes;
    }();
    auto c = uuid{std::as_bytes(std::span(bytes_u8))};

    static_assert(a == b, "test_binary_byte() #1");
    static_assert(bytes_a == bytes_b, "test_binary_byte() #2");
    runtime_assert(a == c, "test_binary_byte() #3");
}

static void test_binary_u8_error()
{
    try {
        constexpr auto a = uuid{S("d604557f-6739-4883-b627-bc0a81b84e97")};

        std::array<uint8_t, 15> bytes;
        a.write_bytes(bytes);
        runtime_assert(0, "test_binary_u8_error() #1");
    }
    catch (std::invalid_argument&) {}

    try {
        constexpr std::array<uint8_t, 15> bytes {
            0xd6,0x04,0x55,0x7f,0x67,0x39,0x48,0x83,
            0xb6,0x27,0xbc,0x0a,0x81,0xb8,0x4e,
        };

        uuid{bytes};
        runtime_assert(0, "test_binary_u8_error() #2");
    }
    catch (std::invalid_argument&) {}
}

template <class Map>
static void test_map_impl()
{
    constexpr auto a = uuid{S("01926c01-ba2c-7315-a16a-0e16d8a51d4c")};
    constexpr auto b = uuid{S("01926c01-ba2c-798b-8dcc-04e45ae6188b")};
    constexpr auto c = uuid{S("01926c01-ba2d-708e-90a1-cc63e523be32")};

    Map m {
        { uuid{S("01926c01-ba2c-7315-a16a-0e16d8a51d4c")}, S("a") },
        { uuid{S("01926c01-ba2c-798b-8dcc-04e45ae6188b")}, S("b") },
        { uuid{S("01926c01-ba2d-708e-90a1-cc63e523be32")}, S("c") },
    };

    runtime_assert(m.size() == 3, "test_map() #1");
    runtime_assert(m.at(a) == S("a"), "test_map() #2");
    runtime_assert(m.at(b) == S("b"), "test_map() #3");
    runtime_assert(m.at(c) == S("c"), "test_map() #4");

    auto [it, insert] = m.insert_or_assign(a, S("x"));
    runtime_assert(!insert, "test_map() #5");
    runtime_assert(m.size() == 3, "test_map() #6");
    runtime_assert(m.at(a) == S("x"), "test_map() #7");
    runtime_assert(m.at(b) == S("b"), "test_map() #8");
    runtime_assert(m.at(c) == S("c"), "test_map() #9");
}

static void test_map()
{
    test_map_impl<std::map<uuid, String>>();
    test_map_impl<std::unordered_map<uuid, String>>();
}

#define V_TO_S(v) #v
#define TO_S(m) V_TO_S(m)

int main(int argc, char** argv)
{
    try {
        test_nil();
        test_compare();
        test_random();
        test_time();
        test_parse();
        test_parse_error();
        test_parse_error_unicode();
        test_string();
        test_string_error();
        test_ostream();
        test_binary_u8();
        test_binary_byte();
        test_binary_u8_error();
        test_map();

        std::cout << "All tests successful.\t"
                  << TO_S(CHAR_T)
                  << std::endl;
        return 0;
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
