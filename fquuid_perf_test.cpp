#include "fquuid.hpp"
#include "fquuid_perf_test.hpp"

class fquuid_impl
{
    fquuid::uuid_generator gen;
    std::mt19937 mt; // [INSECURE] for performance test

public:
    using uuid_type = fquuid::uuid;
    using array_type = std::array<uint8_t, 16>;

    uuid_type gen_v4() { return gen.v4(); }
    uuid_type gen_v7() { return gen.v7(); }

    uuid_type gen_v4_mt() { return gen.v4(mt); }
    uuid_type gen_v7_mt() { return gen.v7(mt); }

    uuid_type parse(const std::string& s) { return uuid_type{s}; }

    std::string to_string(const uuid_type& u) { return u.to_string(); }
    void to_string(const uuid_type& u, std::span<char> s) { u.to_string(s); }

    uuid_type load_bytes(const array_type& a) { return uuid_type{a}; }
    void to_bytes(const uuid_type& u, array_type& a) { u.to_bytes(a); }
};

int main(int argc, char** argv)
{
    fquuid::uuid_perf_test<fquuid_impl> test;
    test.run();
}
