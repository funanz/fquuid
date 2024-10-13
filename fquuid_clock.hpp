#pragma once
#include <chrono>
#include <cstdint>

namespace fquuid
{
    class uuid_clock
    {
    public:
        static int64_t now() {
            auto tp = std::chrono::system_clock::now();
            auto d = tp.time_since_epoch();
            auto dms = std::chrono::duration_cast<std::chrono::milliseconds>(d);
            return dms.count();
        }
    };
}
