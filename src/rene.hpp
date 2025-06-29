#pragma once

#define RENE_NAME         "rené"
#define RENE_VERSION      "25.6.29"
#define RENE_RELEASE      "pre-release 2"
#define RENE_DESCRIPTION  "a powerful & minimalist auto-namer."

namespace rene
{
    auto static const RENE_HELP_TEXT = ut::cstrview(R"(

usage: rene <directory|input file>

rené, a powerful & minimalist auto-namer.

Options:
    -s, --sed        Stream Editor Mode.
    -h, --help       Display this help message and exit.
    -v, --version    Show version information and exit.

For more information, visit: https://github.com/jmitchell24/rene

    )").trim();

    auto static const RENE_VERSION_TEXT = ut::cstrview(R"(

rené, version 25.6.29
Copyright © 2025 James Peter Mitchell
For more information, visit: https://github.com/jmitchell24/rene

    )").trim();
}

/* rene usage (draft)




*/
