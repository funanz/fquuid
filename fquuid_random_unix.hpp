#pragma once
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <sys/random.h>

namespace fquuid
{
    template <class ResultType>
    class uuid_sys_getrandom
    {
    public:
        using result_type = ResultType;
        static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
        static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

        result_type operator ()() {
            result_type r;

            for (;;) {
                auto ret = getrandom(&r, sizeof(r), GRND_NONBLOCK);
                if (ret == -1) {
                    if (errno == EAGAIN || errno == EINTR) {
                        continue;
                    }else
                        throw std::runtime_error(strerror(errno));
                }
                return r;
            }
        }
    };

    using uuid_random = uuid_sys_getrandom<uint64_t>;
}
