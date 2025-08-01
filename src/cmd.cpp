//
// Created by james on 18/04/25.
//

#include "cmd.hpp"

#include "ut/check.hpp"
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

CmdArgs::CmdArgs(int argc, char** argv) :
    m_valid{false},
    m_program_name{}, m_positional_args{},
    m_named_args{}, m_flags{}
{
    parse(argc, argv);
}

bool CmdArgs::valid() const
{
    return m_valid;
}

bool CmdArgs::empty() const
{
    return
        m_positional_args.empty() &&
        m_named_args.empty() &&
        m_flags.empty();
}


string const& CmdArgs::programName() const
{
    return m_program_name;
}

bool CmdArgs::flag(string const& s) const
{
    return m_flags.find(s) != m_flags.end();
}

bool CmdArgs::tryGetPositional(size_t index, string& value) const
{
    if (index < m_positional_args.size())
    {
        value = m_positional_args[index];
        return true;
    }

    return false;
}

bool CmdArgs::tryGetNamed(string const& name, string& value) const
{
    if (auto it = m_named_args.find(name); it != m_named_args.end())
    {
        value = it->second;
        return true;
    }
    return false;
}

bool CmdArgs::parse(int argc, char** argv)
{
    positional_args_type pargs;
    named_args_type nargs;
    flags_type flags;
    string pname;

    m_positional_args.clear();
    m_named_args.clear();
    m_flags.clear();
    m_valid = false;

    if (argc < 1)
    {
        return false;
    }

    check_null(argv[0]);
    pname = argv[0];

    for (size_t i = 1; i < argc; ++i)
    {
        check_null(argv[i]);
        auto arg = cstrview(argv[i]);

        if (arg.beginsWith("--"))
        {
            auto narg = arg.skip(2).str();

            // Check if this is an argument with an equals sign (--key=value)
            auto eq_pos = narg.find('=');
            if (eq_pos != string::npos)
            {
                auto key = narg.substr(0, eq_pos);
                auto value = narg.substr(eq_pos + 1);
                nargs[key] = value;
                continue;
            }

            // Check if next argument exists and doesn't start with '-'
            if (i + 1 < argc && !cstrview(argv[i + 1]).beginsWith("-"))
            {
                nargs[narg] = argv[++i];
            }
            else
            {
                flags.insert(narg);
            }
            continue;
        }

        if (arg.beginsWith("-"))
        {
            auto narg = arg.skip(1).str();

            // Handle single character flags that might be combined (e.g., -abc)
            if (narg.length() > 1 && (i + 1 >= argc || cstrview(argv[i + 1]).beginsWith("-")))
            {
                // Treat as multiple single-character flags
                for (char c : narg)
                {
                    flags.insert(string(1, c));
                }
                continue;
            }

            // Check if next argument exists and doesn't start with '-'
            if (i + 1 < argc && !cstrview(argv[i + 1]).beginsWith("-"))
            {
                nargs[narg] = argv[++i];
            }
            else
            {
                flags.insert(narg);
            }
            continue;
        }

        // Positional argument
        pargs.push_back(arg.str());
    }

    m_program_name      = pname;
    m_positional_args   = move(pargs);
    m_named_args        = move(nargs);
    m_flags             = move(flags);
    m_valid             = true;
    return true;
}

void CmdArgs::dbgPrint()
{
    using namespace ut::tui;



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


























