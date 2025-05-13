
//
// rene
//
#include <unistd.h>

#include "cmd.hpp"
#include "user_interface.hpp"
using namespace rene;

//
// ut
//
#include <ut/ut.hpp>
#include <ut/term.hpp>
using namespace ut;

//
// std
//
#include <iostream>
using namespace std;

#include <string>


auto g_help_text =(R"(
usage: rene <directory>

René, a tui bulk file renamer

Options:
    -h, --help       Display this help message and exit.
    -v, --version    Show version information and exit.

For more information, visit: https://github.com/jmitchell24/rene
)"_sv).trim();

auto g_version_text = (R"(
René, version 25.5.9 'algoma'
Copyright © 2025 James Peter Mitchell
For more information, visit: https://github.com/jmitchell24/rene
)"_sv).trim();

int main(int argc, char** argv)
{
    CmdArgs args{argc, argv};

    if (args.flagHelp())
    {
        cout << g_help_text << endl;
        return EXIT_SUCCESS;
    }

    if (args.flagVersion())
    {
        cout << g_version_text << endl;
        return EXIT_SUCCESS;
    }

    if (string path; args.tryGetPositional(0, path))
    {
        return UserInterface::instance().run(path);
    }

    array<char, 1000> cwdbuf;
    getcwd(cwdbuf.data(), cwdbuf.size());
    filesystem::path cwd = cwdbuf.data();
    return UserInterface::instance().run(cwd);
}

