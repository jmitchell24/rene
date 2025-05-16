
//
// rene
//
#include <unistd.h>

#include "cmd.hpp"
#include "rene.hpp"
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




int main(int argc, char** argv)
{
    CmdArgs args{argc, argv};

    if (args.flagHelp())
    {
        cout << RENE_HELP_TEXT << endl;
        return EXIT_SUCCESS;
    }

    if (args.flagVersion())
    {
        cout << RENE_VERSION_TEXT << endl;
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

