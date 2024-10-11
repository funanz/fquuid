#pragma once
#define NOMINMAX
#define WIN32_NO_STATUS
#include <windows.h>
#undef NOMINMAX
#undef WIN32_NO_STATUS

#include <winternl.h>
#include <ntstatus.h>
#include <winerror.h>
#include <bcrypt.h>
#include <cstdint>
#include <limits>

namespace fquuid
{
    template <class ResultType>
    class uuid_bcrypt_gen_random
    {
    public:
        using result_type = ResultType;
        static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
        static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

        result_type operator ()() {
            result_type r;

            auto status = BCryptGenRandom(
                nullptr, reinterpret_cast<PUCHAR>(&r), sizeof(r),
                BCRYPT_USE_SYSTEM_PREFERRED_RNG);

            if (!NT_SUCCESS(status))
                throw std::runtime_error("error: BCryptGenRandom()");

            return r;
        }
    };

    using uuid_random = uuid_bcrypt_gen_random<uint64_t>;
}
