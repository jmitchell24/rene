//
// Created by james on 04/05/25.
//

#pragma once

#include <string>
#include <variant>
#include <vector>

#include "ut/check.hpp"

namespace rene
{
    struct Var;
    struct VarLiteral { std::string text; };
    struct VarOriginal { };
    struct VarIncrement { };

    using varlist_type = std::vector<Var>;

    struct Var
    {
        enum Kind { EMPTY, LITERAL, ORIGINAL, INCREMENT };

        using var_type = std::variant<
            std::monostate, VarLiteral, VarOriginal, VarIncrement>;

        inline VarLiteral const& asLiteral() const { check(kind() == LITERAL); return std::get<LITERAL>(data); }

        inline Kind kind() const { return static_cast<Kind>(data.index()); }
        var_type data;
    };




    class Replacer
    {
    public:

        struct Args
        {
            std::string original;
            int increment;
        };

        Replacer()=default;
        Replacer(std::string const& expr);

        std::string replace(Args const& args) const;

    private:
        varlist_type m_vars;
    };
}
