//
// Created by james on 18/04/25.
//

#pragma once

//
// ut
//
#include <ut/string.hpp>

//
// std
//
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace rene
{


    ///
    class CmdArgs
    {
    public:
        using named_args_type       = std::unordered_map<std::string, std::string>;
        using flags_type            = std::unordered_set<std::string>;
        using positional_args_type  = std::vector<std::string>;

        CmdArgs(int argc, char** argv);

        bool valid() const;
        bool empty() const;

        inline bool flagHelp() const { return anyFlag("help", "h"); }
        inline bool flagVersion() const { return anyFlag("version", "v"); }
        inline bool flagSedMode() const { return anyFlag("sed", "s"); }

        std::string const& programName() const;
        bool flag(std::string const& s) const;
        bool tryGetPositional(size_t index, std::string& value) const;
        bool tryGetNamed(std::string const& name, std::string& value) const;

        template <typename... Args>
        bool anyFlag(Args&&... args) const
        { return (flag(args) || ...); }


        bool parse(int argc, char** argv);
        void dbgPrint();

    private:
        bool                    m_valid;
        std::string             m_program_name;
        positional_args_type    m_positional_args;
        named_args_type         m_named_args;
        flags_type              m_flags;

    };
}