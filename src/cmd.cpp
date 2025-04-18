//
// Created by james on 18/04/25.
//

#include "cmd.hpp"
using namespace rene;

//
// ut
//
#include <ut/term.hpp>
using namespace ut;

//
// std
//
#include <iostream>
using namespace std;

//
// CmdArgs -> implementation
//

string const& CmdArgs::programName() const
{
    return m_program_name;
}

bool CmdArgs::flag(string const& s, char c) const
{
    return m_flags.contains(s) || m_flags.contains(string(1, c));
}

bool CmdArgs::parse(int argc, char** argv)
{
    positional_args_type pargs;
    named_args_type nargs;
    string pname;

    if (argc < 1)
        return false;

    pname = argv[0];

    for (size_t i = 1; i < argc; ++i)
    {
        auto arg = cstrview(argv[i]);

        if (arg.beginsWith("--"))
        {
            auto narg = arg.skip(2).str();
            if (++i < argc)
                nargs[narg] = argv[i];
            else
                m_flags.insert(narg);
            continue;
        }

        if (arg.beginsWith("-"))
        {
            auto narg = arg.skip(1).str();
            if (++i < argc)
                nargs[narg] = argv[i];
            else
                m_flags.insert(narg);
            continue;
        }

        pargs.push_back(arg.str());
    }

    m_program_name      = pname;
    m_positional_args   = move(pargs);
    m_named_args        = move(nargs);
    return true;
}

void CmdArgs::dbgPrint()
{
    cout << endl;
    cout << TERM_FG_BRIGHT_BLUE << "  program name: " << TERM_RESET << m_program_name << endl;
    cout << TERM_FG_BRIGHT_BLUE << "  positional: " << TERM_RESET << endl;

    for (size_t i = 0; i < m_positional_args.size(); ++i)
        cout << "    " << TERM_FG_BRIGHT_YELLOW << i << ": " << TERM_RESET << m_positional_args[i] << endl;

    cout << TERM_FG_BRIGHT_BLUE << "  named: " << TERM_RESET << endl;

    for (auto&& [k,v]: m_named_args)
        cout << "    " << TERM_FG_BRIGHT_YELLOW << k << ": " << TERM_RESET << v << endl;


    cout << TERM_FG_BRIGHT_BLUE << "  flags: " << TERM_RESET << endl;

    for (auto&& it: m_flags)
        cout << "    " << TERM_FG_BRIGHT_YELLOW << it << TERM_RESET << endl;

}
