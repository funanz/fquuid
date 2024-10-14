// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#include <iostream>
#include <span>
#include <string>
#include "fquuid.hpp"

static int usage(const std::string& cmd)
{
    std::cout << "Usage: " << cmd << " [options]\n";
    std::cout << R"(
Options:
 -4     generate UUIDv4 (default)
 -7     generate UUIDv7
 -i     infinite loop
)";

    return 0;
}

static int unknown_option(const std::string& cmd, const std::string& arg)
{
    std::cerr << cmd << ": unknown option '" << arg << "'\n";

    return 1;
}

enum class gen_mode {
    v4,
    v7,
};

int main(int argc, char** argv)
{
    gen_mode mode = gen_mode::v4;
    bool inf_loop = false;

    auto args = std::span(argv, argc);
    for (std::string arg : args.subspan(1)) {
        if (arg == "-4")
            mode = gen_mode::v4;
        else if (arg == "-7")
            mode = gen_mode::v7;
        else if (arg == "-i")
            inf_loop = true;
        else if (arg == "-h" || arg == "--help")
            return usage(args[0]);
        else
            return unknown_option(args[0], arg);
    }

    fquuid::uuid_random rng;
    do {
        if (mode == gen_mode::v4)
            fquuid::uuid_generator_v4::generate(rng).println();
        else if (mode == gen_mode::v7)
            fquuid::uuid_generator_v7::generate(rng).println();
        else
            break;
    } while (inf_loop);

    return 0;
}
