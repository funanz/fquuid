# Fast? UUID Generator

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ make test
    $ ./fquuid-perf-test
     0:03.009300855	102.68 M op/s	parse
     0:03.002983872	 80.59 M op/s	to string
     0:03.004863570	160.41 M op/s	to string (array)
    ...
    $ ./fquuidgen
    2c8ae67b-3916-4097-ac28-ae3ed37fa562

Using the latest boost

    $ cmake -D CMAKE_INCLUDE_PATH=~/boost-1.86.0 ..

# Results

|Tests|fquuid(Linux)|fquuid(Windows)|boost-1.86.0(Linux)|
|:---|---:|---:|---:|
|parse|102.68 M op/s| 27.31 M op/s|  3.61 M op/s|
|to string| 80.59 M op/s| 23.84 M op/s| 71.98 M op/s|
|to string (array)|160.41 M op/s|118.27 M op/s|             |
|load bytes|  1.26 G op/s|199.19 M op/s|  1.14 G op/s|
|to bytes|  1.03 G op/s|405.21 M op/s|  1.13 G op/s|
|compare|483.12 M op/s|337.82 M op/s|411.89 M op/s|
|generate v4 (mt19937)| 95.57 M op/s| 97.83 M op/s| 94.58 M op/s|
|generate v7 (mt19937)| 47.84 M op/s| 40.25 M op/s|             |
|generate v4 (default)| 22.78 M op/s| 13.88 M op/s| 83.33 M op/s|
|generate v7 (default)| 18.12 M op/s| 11.54 M op/s| 44.04 M op/s|
|generate v4 (default, std::set)|  1.34 M op/s|  1.10 M op/s|  1.19 M op/s|
|generate v7 (default, std::set)|  5.22 M op/s|  4.59 M op/s|  4.17 M op/s|
|generate v4 (default, std::unordered_set)|  2.03 M op/s|  2.72 M op/s|  2.33 M op/s|
|generate v7 (default, std::unordered_set)|  2.19 M op/s|  2.59 M op/s|  2.16 M op/s|

# Remarks

- parse は早いがコンパイル生成コードの配置によっては遅くなる
- boost-1.86.0 から乱数がすごい高速になった
- v7 はクロックの取得がボトルネックになる
- 高速クロックは早いが、精度が0.1～1.0秒なので使用は難しい
