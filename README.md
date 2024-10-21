# Fast? UUID Generator

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ make test
    $ ./fquuid-perf-test
     0:03.007403094	103.08 M op/s	parse
     0:03.009803045	 76.08 M op/s	to string
     0:03.006354331	158.33 M op/s	to string (array)
    ...
    $ ./fquuidgen
    2c8ae67b-3916-4097-ac28-ae3ed37fa562

Using the latest boost

    $ cmake -D CMAKE_REQUIRED_INCLUDES=~/boost-1.86.0 ..

# Results

<details>
<summary>Linux x86_64 (WSL2) gcc-14.2.1</summary>

| Tests                                     |        fquuid |  boost-1.86.0 |
|:------------------------------------------|--------------:|--------------:|
| parse                                     | 103.08 M op/s |   3.60 M op/s |
| to string                                 |  76.08 M op/s |  63.95 M op/s |
| to string (array)                         | 158.33 M op/s |               |
| load bytes                                |   1.15 G op/s |   1.00 G op/s |
| to bytes                                  | 929.53 M op/s | 986.59 M op/s |
| compare                                   | 479.77 M op/s | 633.44 M op/s |
| generate v4 (mt19937)                     |  99.61 M op/s |  94.68 M op/s |
| generate v7 (mt19937)                     |  47.88 M op/s |               |
| generate v4 (default)                     |  22.81 M op/s |  83.51 M op/s |
| generate v7 (default)                     |  18.10 M op/s |  44.13 M op/s |
| generate v4 (default, std::set)           |   1.27 M op/s |   1.23 M op/s |
| generate v7 (default, std::set)           |   5.06 M op/s |   4.28 M op/s |
| generate v4 (default, std::unordered_set) |   2.22 M op/s |   2.47 M op/s |
| generate v7 (default, std::unordered_set) |   2.26 M op/s |   2.31 M op/s |

</details>

<details>
<summary>Windows 11 x64 VS2022-17.11.5</summary>

| Tests                                     |        fquuid |  boost-1.86.0 |
|:------------------------------------------|--------------:|--------------:|
| parse                                     |  38.30 M op/s |  14.16 M op/s |
| to string                                 |  24.22 M op/s |  29.00 M op/s |
| to string (array)                         | 122.81 M op/s |               |
| load bytes                                | 218.33 M op/s | 476.26 M op/s |
| to bytes                                  | 509.60 M op/s | 560.22 M op/s |
| compare                                   | 312.54 M op/s | 298.59 M op/s |
| generate v4 (mt19937)                     |  89.10 M op/s |  97.29 M op/s |
| generate v7 (mt19937)                     |  39.70 M op/s |               |
| generate v4 (default)                     |  13.47 M op/s |  46.62 M op/s |
| generate v7 (default)                     |  11.23 M op/s |  33.17 M op/s |
| generate v4 (default, std::set)           |   1.09 M op/s |   1.15 M op/s |
| generate v7 (default, std::set)           |   3.65 M op/s |   5.13 M op/s |
| generate v4 (default, std::unordered_set) |   2.42 M op/s |   2.89 M op/s |
| generate v7 (default, std::unordered_set) |   2.31 M op/s |   2.87 M op/s |

</details>

# Example

```C++
#include <fquuid.hpp>
#include <map>
#include <string>

using namespace fquuid;

void example()
{
    // constexpr
    constexpr auto x = uuid{"c66b4959-8c77-4b11-9763-270490f684e2"};

    // generate
    uuid_generator_v7 gen_v7;
    auto y = gen_v7();

    // change RNG
    std::random_device rng;
    auto z = uuid_generator_v7::generate(rng);

    // map
    std::map<uuid, std::string> m {
        { gen_v7(), "hoge" },
        { gen_v7(), "fuga" },
        { gen_v7(), "piyo" },
    };

    // string
    std::string s = x.to_string();

    // bytes
    std::array<uint8_t, 16> a;
    x.to_bytes(a);
}
```

# Remarks

- parse は早いがコンパイル生成コードの配置によっては遅くなる
- boost-1.86.0 から乱数がすごい高速になった
- v7 はクロックの取得がボトルネックになる
- 高速クロックは早いが、精度が0.1～1.0秒なので使用は難しい
