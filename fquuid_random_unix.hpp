// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstdint>
#include <cstdio>
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

    template <class ResultType>
    class uuid_dev_urandom
    {
        FILE *file_;

    public:
        using result_type = ResultType;
        static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
        static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

        uuid_dev_urandom() {
            file_ = nullptr;
        }

        ~uuid_dev_urandom() {
            close();
        }

        void open() {
            if (file_ != nullptr) return;

            file_ = fopen("/dev/urandom", "r");
            if (file_ == nullptr)
                throw std::runtime_error("uuid_dev_urandom: can't open file");
        }

        void close() {
            if (file_ != nullptr) {
                fclose(file_);
                file_ = nullptr;
            }
        }

        result_type operator ()() {
            if (file_ == nullptr)
                open();

            result_type r;
            auto ret = fread(&r, sizeof(r), 1, file_);
            if (ret == 0)
                throw std::runtime_error("uuid_dev_urandom: read error");
            return r;
        }

        uuid_dev_urandom(uuid_dev_urandom&& r) {
            file_ = r.file_;
            r.file_ = nullptr;
        }

        uuid_dev_urandom& operator =(uuid_dev_urandom&& r) {
            close();
            file_ = r.file_;
            r.file_ = nullptr;
            return *this;
        }

        uuid_dev_urandom(const uuid_dev_urandom&) = delete;
        uuid_dev_urandom& operator =(const uuid_dev_urandom&) = delete;
    };

    using uuid_random = uuid_dev_urandom<uint64_t>;
}
