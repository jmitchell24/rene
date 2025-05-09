//
// Created by james on 04/05/25.
//

#pragma once

#include <string>
#include <variant>
#include <vector>

#include "ut/check.hpp"

#define EXPAND_VARS(_var) \
    _var(VarLiteral, LITERAL) \
    _var(VarOriginal, ORIGINAL) \
    _var(VarIncrement, INCREMENT) \
    _var(VarFuzz, FUZZ)

#define VAR_ENUM(_x, _y) ,_y
#define VAR_VARIANT(_x, _y) ,_x

namespace rene
{
    struct Var;
    struct VarLiteral   { std::string text; };
    struct VarOriginal  { };
    struct VarIncrement { };
    struct VarFuzz      { };

    using varlist_type = std::vector<Var>;

    struct Var
    {
        enum Kind{ EMPTY EXPAND_VARS(VAR_ENUM) };
        using var_type = std::variant<std::monostate EXPAND_VARS(VAR_VARIANT)>;

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
