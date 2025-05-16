#pragma once

#define RENE_NAME         "rené"
#define RENE_VERSION      "25.5.16"
#define RENE_RELEASE      "pre-release"
#define RENE_DESCRIPTION  "@PROJECT_DESCRIPTION"

namespace rene
{
    auto static const RENE_HELP_TEXT = ut::cstrview(R"(

usage: rene <directory>

rené, a powerful & minimalist auto-namer.

Options:
    -h, --help       Display this help message and exit.
    -v, --version    Show version information and exit.

For more information, visit: https://github.com/jmitchell24/rene

    )").trim();

    auto static const RENE_VERSION_TEXT = ut::cstrview(R"(

rené, version 25.5.16
Copyright © 2025 James Peter Mitchell
For more information, visit: https://github.com/jmitchell24/rene

    )").trim();
}
