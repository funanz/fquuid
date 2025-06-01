// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#include <fquuid.hpp>
#include "fquuid_perf_test.hpp"

class fquuid_impl
{
    fquuid::uuid_generator_v4 v4;
    fquuid::uuid_generator_v7 v7;
    std::mt19937 mt; // [INSECURE] for performance test

public:
    using uuid_type = fquuid::uuid;
    using array_type = std::array<uint8_t, 16>;

    uuid_type gen_v4() { return v4(); }
    uuid_type gen_v7() { return v7(); }

    uuid_type gen_v4_mt() { return fquuid::uuid_generator_v4::generate(mt); }
    uuid_type gen_v7_mt() { return fquuid::uuid_generator_v7::generate(mt); }

    uuid_type parse(const std::string& s) { return uuid_type{s}; }

    std::string to_string(const uuid_type& u) { return u.to_string(); }
    void to_string(const uuid_type& u, std::span<char> s) { u.write_string_without_null(s); }

    uuid_type load_bytes(const array_type& a) { return uuid_type{a}; }
    void to_bytes(const uuid_type& u, array_type& a) { u.write_bytes(a); }
};

int main(int argc, char** argv)
{
    fquuid::uuid_perf_test<fquuid_impl> test;
    test.run();
}
