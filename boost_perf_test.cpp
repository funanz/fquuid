#include <boost/version.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "fquuid_perf_test.hpp"

#if BOOST_VERSION < 108600 // 1.86.0
namespace std
{
    template<>
    struct hash<boost::uuids::uuid>
    {
        std::size_t operator ()(const boost::uuids::uuid& u) const noexcept {
            return boost::uuids::hash_value(u);
        }
    };
}
#endif

class boost_impl
{
    boost::uuids::random_generator v4;
#if BOOST_VERSION >= 108600 // 1.86.0
    boost::uuids::time_generator_v7 v7;
#endif
    boost::uuids::random_generator_mt19937 mt;
    boost::uuids::string_generator sg;

public:
    using uuid_type = boost::uuids::uuid;
    using array_type = std::array<uint8_t, 16>;

    uuid_type gen_v4() { return v4(); }
#if BOOST_VERSION >= 108600 // 1.86.0
    uuid_type gen_v7() { return v7(); }
#else
    uuid_type gen_v7() { throw fquuid::not_implemented(); }
#endif

    uuid_type gen_v4_mt() { return mt(); }
    uuid_type gen_v7_mt() { throw fquuid::not_implemented(); }

    uuid_type parse(const std::string& s) { return sg(s); }

    std::string to_string(const uuid_type& u) {
        return boost::uuids::to_string(u);
    }

    void to_string(const uuid_type& u, std::span<char> s) {
        throw fquuid::not_implemented();
    }

    uuid_type load_bytes(const array_type& a) {
        uuid_type u;
        std::copy(a.begin(), a.end(), u.begin());
        return u;
    }

    void to_bytes(const uuid_type& u, array_type& a) {
        std::copy(u.begin(), u.end(), a.begin());
    }
};

int main(int argc, char** argv)
{
    fquuid::uuid_perf_test<boost_impl> test;
    test.run();
}
