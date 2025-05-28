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

    constexpr const char* ptr = "d604557f-6739-4883-b627-bc0a81b84e97";
    constexpr auto f = uuid{ptr};

    std::string s = "d604557f-6739-4883-b627-bc0a81b84e97";
    auto g = uuid{s};

    static_assert(a == b, "test_parse() #1");
    static_assert(b == c, "test_parse() #2");
    static_assert(c == d, "test_parse() #3");
    static_assert(d == e, "test_parse() #4");
    static_assert(e == f, "test_parse() #5");
    runtime_assert(f == g, "test_parse() #6");
}

static void test_parse_w()
{
    constexpr auto a = uuid{L"d604557f-6739-4883-b627-bc0a81b84e97"};
    constexpr auto b = uuid{L"d604557f67394883b627bc0a81b84e9\U00000037"};

    constexpr auto span = std::span(L"d604557f-6739-4883-b627-bc0a81b84e97");
    constexpr auto c = uuid{span};

    constexpr const wchar_t* ptr = L"d604557f-6739-4883-b627-bc0a81b84e97";
    constexpr auto d = uuid{ptr};

    std::wstring s = L"d604557f-6739-4883-b627-bc0a81b84e97";
    auto e = uuid{s};

    static_assert(a == b, "test_parse_w() #1");
    static_assert(b == c, "test_parse_w() #2");
    static_assert(c == d, "test_parse_w() #3");
    runtime_assert(d == e, "test_parse_w() #4");
}

static void test_parse_u8()
{
    constexpr auto a = uuid{u8"d604557f-6739-4883-b627-bc0a81b84e97"};
    constexpr auto b = uuid{u8"d604557f67394883b627bc0a81b84e9\U00000037"};

    constexpr auto span = std::span(u8"d604557f-6739-4883-b627-bc0a81b84e97");
    constexpr auto c = uuid{span};

    constexpr const char8_t* ptr = u8"d604557f-6739-4883-b627-bc0a81b84e97";
    constexpr auto d = uuid{ptr};

    std::u8string s = u8"d604557f-6739-4883-b627-bc0a81b84e97";
    auto e = uuid{s};

    static_assert(a == b, "test_parse_u8() #1");
    static_assert(b == c, "test_parse_u8() #2");
    static_assert(c == d, "test_parse_u8() #3");
    runtime_assert(d == e, "test_parse_u8() #4");
}

static void test_parse_u16()
{
    constexpr auto a = uuid{u"d604557f-6739-4883-b627-bc0a81b84e97"};
    constexpr auto b = uuid{u"d604557f67394883b627bc0a81b84e9\U00000037"};

    constexpr auto span = std::span(u"d604557f-6739-4883-b627-bc0a81b84e97");
    constexpr auto c = uuid{span};

    constexpr const char16_t* ptr = u"d604557f-6739-4883-b627-bc0a81b84e97";
    constexpr auto d = uuid{ptr};

    std::u16string s = u"d604557f-6739-4883-b627-bc0a81b84e97";
    auto e = uuid{s};

    static_assert(a == b, "test_parse_u16() #1");
    static_assert(b == c, "test_parse_u16() #2");
    static_assert(c == d, "test_parse_u16() #3");
    runtime_assert(d == e, "test_parse_u16() #4");
}

static void test_parse_u32()
{
    constexpr auto a = uuid{U"d604557f-6739-4883-b627-bc0a81b84e97"};
    constexpr auto b = uuid{U"d604557f67394883b627bc0a81b84e9\U00000037"};

    constexpr auto span = std::span(U"d604557f-6739-4883-b627-bc0a81b84e97");
    constexpr auto c = uuid{span};

    constexpr const char32_t* ptr = U"d604557f-6739-4883-b627-bc0a81b84e97";
    constexpr auto d = uuid{ptr};

    std::u32string s = U"d604557f-6739-4883-b627-bc0a81b84e97";
    auto e = uuid{s};

    static_assert(a == b, "test_parse_u32() #1");
    static_assert(b == c, "test_parse_u32() #2");
    static_assert(c == d, "test_parse_u32() #3");
    runtime_assert(d == e, "test_parse_u32() #4");
}

static void test_parse_error()
{
    try {
        const char* ptr = nullptr;
        uuid{ptr};
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

    // load_u16_hex 000x
    try {
        uuid{"d604557f-673x-4883-b627-bc0a81b84e97"};
        runtime_assert(0, "test_parse_error() #6");
    }
    catch (std::invalid_argument&) {}

    // load_u16_hex x000
    try {
        uuid{"d604557f-x739-4883-b627-bc0a81b84e97"};
        runtime_assert(0, "test_parse_error() #7");
    }
    catch (std::invalid_argument&) {}

    // load_u32_hex 00000000x
    try {
        uuid{"d604557x-6739-4883-b627-bc0a81b84e97"};
        runtime_assert(0, "test_parse_error() #8");
    }
    catch (std::invalid_argument&) {}

    // load_u32_hex x00000000
    try {
        uuid{"x604557f-6739-4883-b627-bc0a81b84e97"};
        runtime_assert(0, "test_parse_error() #9");
    }
    catch (std::invalid_argument&) {}

    // load_u48_hex 00000000000x
    try {
        uuid{"d604557f-6739-4883-b627-bc0a81b84e9x"};
        runtime_assert(0, "test_parse_error() #10");
    }
    catch (std::invalid_argument&) {}

    // load_u48_hex x00000000000
    try {
        uuid{"d604557f-6739-4883-b627-xc0a81b84e97"};
        runtime_assert(0, "test_parse_error() #11");
    }
    catch (std::invalid_argument&) {}
}

static void test_parse_w_error()
{
    try {
        uuid{L"d604557f-6739-4883-b627-bc0a81b84e9\U00000137"};
        runtime_assert(0, "test_parse_w_error() #1");
    }
    catch (std::invalid_argument&) {}
}

static void test_parse_u8_error()
{
    try {
        uuid{u8"d604557f-6739-4883-b627-bc0a81b84e9\U00000137"};
        runtime_assert(0, "test_parse_u8_error() #1");
    }
    catch (std::invalid_argument&) {}
}

static void test_parse_u16_error()
{
    try {
        uuid{u"d604557f-6739-4883-b627-bc0a81b84e9\U00000137"};
        runtime_assert(0, "test_parse_u16_error() #1");
    }
    catch (std::invalid_argument&) {}
}

static void test_parse_u32_error()
{
    try {
        uuid{U"d604557f-6739-4883-b627-bc0a81b84e9\U00000137"};
        runtime_assert(0, "test_parse_u32_error() #1");
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

static void test_string_w()
{
    constexpr auto a = uuid{L"d604557f67394883b627bc0a81b84e97"};

    auto s1 = a.to_wstring();

    constexpr auto s2_array = [&] {
        std::array<wchar_t, 40> buf;
        std::ranges::fill(buf, L'*');
        a.to_string_z(buf);
        return buf;
    }();
    std::wstring s2(s2_array.data());

    constexpr auto s3_array = [&] {
        std::array<wchar_t, 40> buf;
        std::ranges::fill(buf, L'*');
        a.to_string(buf);
        return buf;
    }();
    std::wstring s3(s3_array.data(), s3_array.size());

    runtime_assert(s1 == L"d604557f-6739-4883-b627-bc0a81b84e97", "test_string_w() #1");
    runtime_assert(s2 == L"d604557f-6739-4883-b627-bc0a81b84e97", "test_string_w() #2");
    runtime_assert(s3 == L"d604557f-6739-4883-b627-bc0a81b84e97****", "test_string_w() #3");
    static_assert(s2_array.size() == 40, "test_string_w() #4");
    static_assert(s3_array.size() == 40, "test_string_w() #5");
}

static void test_string_u8()
{
    constexpr auto a = uuid{u8"d604557f67394883b627bc0a81b84e97"};

    auto s1 = a.to_u8string();

    constexpr auto s2_array = [&] {
        std::array<char8_t, 40> buf;
        std::ranges::fill(buf, u8'*');
        a.to_string_z(buf);
        return buf;
    }();
    std::u8string s2(s2_array.data());

    constexpr auto s3_array = [&] {
        std::array<char8_t, 40> buf;
        std::ranges::fill(buf, u8'*');
        a.to_string(buf);
        return buf;
    }();
    std::u8string s3(s3_array.data(), s3_array.size());

    runtime_assert(s1 == u8"d604557f-6739-4883-b627-bc0a81b84e97", "test_string_u8() #1");
    runtime_assert(s2 == u8"d604557f-6739-4883-b627-bc0a81b84e97", "test_string_u8() #2");
    runtime_assert(s3 == u8"d604557f-6739-4883-b627-bc0a81b84e97****", "test_string_u8() #3");
    static_assert(s2_array.size() == 40, "test_string_u8() #4");
    static_assert(s3_array.size() == 40, "test_string_u8() #5");
}

static void test_string_u16()
{
    constexpr auto a = uuid{u"d604557f67394883b627bc0a81b84e97"};

    auto s1 = a.to_u16string();

    constexpr auto s2_array = [&] {
        std::array<char16_t, 40> buf;
        std::ranges::fill(buf, u'*');
        a.to_string_z(buf);
        return buf;
    }();
    std::u16string s2(s2_array.data());

    constexpr auto s3_array = [&] {
        std::array<char16_t, 40> buf;
        std::ranges::fill(buf, u'*');
        a.to_string(buf);
        return buf;
    }();
    std::u16string s3(s3_array.data(), s3_array.size());

    runtime_assert(s1 == u"d604557f-6739-4883-b627-bc0a81b84e97", "test_string_u16() #1");
    runtime_assert(s2 == u"d604557f-6739-4883-b627-bc0a81b84e97", "test_string_u16() #2");
    runtime_assert(s3 == u"d604557f-6739-4883-b627-bc0a81b84e97****", "test_string_u16() #3");
    static_assert(s2_array.size() == 40, "test_string_u16() #4");
    static_assert(s3_array.size() == 40, "test_string_u16() #5");
}

static void test_string_u32()
{
    constexpr auto a = uuid{U"d604557f67394883b627bc0a81b84e97"};

    auto s1 = a.to_u32string();

    constexpr auto s2_array = [&] {
        std::array<char32_t, 40> buf;
        std::ranges::fill(buf, u'*');
        a.to_string_z(buf);
        return buf;
    }();
    std::u32string s2(s2_array.data());

    constexpr auto s3_array = [&] {
        std::array<char32_t, 40> buf;
        std::ranges::fill(buf, u'*');
        a.to_string(buf);
        return buf;
    }();
    std::u32string s3(s3_array.data(), s3_array.size());

    runtime_assert(s1 == U"d604557f-6739-4883-b627-bc0a81b84e97", "test_string_u32() #1");
    runtime_assert(s2 == U"d604557f-6739-4883-b627-bc0a81b84e97", "test_string_u32() #2");
    runtime_assert(s3 == U"d604557f-6739-4883-b627-bc0a81b84e97****", "test_string_u32() #3");
    static_assert(s2_array.size() == 40, "test_string_u32() #4");
    static_assert(s3_array.size() == 40, "test_string_u32() #5");
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

static void test_string_w_error()
{
    try {
        constexpr auto a = uuid{L"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<wchar_t, 35> buf;
        a.to_string(buf);
        runtime_assert(0, "test_string_w_error() #1");
    }
    catch (std::invalid_argument&) {}

    try {
        constexpr auto a = uuid{L"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<wchar_t, 36> buf;
        a.to_string_z(buf);
        runtime_assert(0, "test_string_w_error() #2");
    }
    catch (std::invalid_argument&) {}
}

static void test_string_u8_error()
{
    try {
        constexpr auto a = uuid{u8"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<char8_t, 35> buf;
        a.to_string(buf);
        runtime_assert(0, "test_string_u8_error() #1");
    }
    catch (std::invalid_argument&) {}

    try {
        constexpr auto a = uuid{u8"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<char8_t, 36> buf;
        a.to_string_z(buf);
        runtime_assert(0, "test_string_u8_error() #2");
    }
    catch (std::invalid_argument&) {}
}

static void test_string_u16_error()
{
    try {
        constexpr auto a = uuid{u"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<char16_t, 35> buf;
        a.to_string(buf);
        runtime_assert(0, "test_string_u16_error() #1");
    }
    catch (std::invalid_argument&) {}

    try {
        constexpr auto a = uuid{u"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<char16_t, 36> buf;
        a.to_string_z(buf);
        runtime_assert(0, "test_string_u16_error() #2");
    }
    catch (std::invalid_argument&) {}
}

static void test_string_u32_error()
{
    try {
        constexpr auto a = uuid{U"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<char32_t, 35> buf;
        a.to_string(buf);
        runtime_assert(0, "test_string_u32_error() #1");
    }
    catch (std::invalid_argument&) {}

    try {
        constexpr auto a = uuid{U"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<char32_t, 36> buf;
        a.to_string_z(buf);
        runtime_assert(0, "test_string_u32_error() #2");
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

static void test_ostream_w()
{
    constexpr auto a = uuid{L"d604557f67394883b627bc0a81b84e97"};

    std::wostringstream oss;
    oss << "{" << a << "}";

    runtime_assert(oss.str() == L"{d604557f-6739-4883-b627-bc0a81b84e97}", "test_ostream() #1");
}

static void test_binary_u8()
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

    static_assert(a == b, "test_binary_u8() #1");
    static_assert(bytes_a == bytes_b, "test_binary_u8() #2");
}

static void test_binary_byte()
{
    constexpr auto a = uuid{"d604557f-6739-4883-b627-bc0a81b84e97"};

    constexpr std::array<std::byte, 16> bytes_a {
        std::byte(0xd6),std::byte(0x04),std::byte(0x55),std::byte(0x7f),
        std::byte(0x67),std::byte(0x39),std::byte(0x48),std::byte(0x83),
        std::byte(0xb6),std::byte(0x27),std::byte(0xbc),std::byte(0x0a),
        std::byte(0x81),std::byte(0xb8),std::byte(0x4e),std::byte(0x97),
    };

    constexpr auto b = uuid{bytes_a};

    constexpr auto bytes_b = [&] {
        std::array<std::byte, 16> bytes;
        a.to_bytes(bytes);
        return bytes;
    }();

    constexpr auto bytes_u8 = [&] {
        std::array<uint8_t, 16> bytes;
        a.to_bytes(bytes);
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
        constexpr auto a = uuid{"d604557f-6739-4883-b627-bc0a81b84e97"};

        std::array<uint8_t, 15> bytes;
        a.to_bytes(bytes);
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
        test_parse_w();
        test_parse_u8();
        test_parse_u16();
        test_parse_u32();
        test_parse_error();
        test_parse_w_error();
        test_parse_u8_error();
        test_parse_u16_error();
        test_parse_u32_error();
        test_string();
        test_string_w();
        test_string_u8();
        test_string_u16();
        test_string_u32();
        test_string_error();
        test_string_w_error();
        test_string_u8_error();
        test_string_u16_error();
        test_string_u32_error();
        test_ostream();
        test_ostream_w();
        test_binary_u8();
        test_binary_byte();
        test_binary_u8_error();
        test_map();

        std::cout << "All tests successful" << std::endl;
        return 0;
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
