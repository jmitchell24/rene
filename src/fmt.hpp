//
// Created by james on 04/05/25.
//

#pragma once

//
// std
//
#include <string>
#include <variant>
#include <vector>

//
// ut
//
#include <ut/string.hpp>
#include <ut/check.hpp>

#define EXPAND_VARS(_var) \
    _var(Literal, LITERAL) \
    _var(Original, ORIGINAL) \
    _var(OriginalName, ORIGINAL_NAME) \
    _var(OriginalExt, ORIGINAL_EXT) \
    _var(Index, INDEX) \
    _var(Fuzz, FUZZ)

#define VAR_ENUM(_x, _y) ,_y
#define VAR_VARIANT(_x, _y) ,Var##_x

#define VAR_GETTER(_x, _y) inline Var##_x const& as##_x() const { check(kind() == _y); return std::get<_y>(data); }

namespace rene::fmt
{
    struct Var;
    struct VarLiteral   { std::string text; };
    struct VarOriginal  { };
    struct VarOriginalName { };
    struct VarOriginalExt { };
    struct VarIndex { int offset; };
    struct VarFuzz      { };

    using varlist_type = std::vector<Var>;

    struct Var
    {
        enum Kind{ EMPTY EXPAND_VARS(VAR_ENUM) };
        using var_type = std::variant<std::monostate EXPAND_VARS(VAR_VARIANT)>;
        EXPAND_VARS(VAR_GETTER)

        inline Kind kind() const { return static_cast<Kind>(data.index()); }
        var_type data;
    };

    struct State
    {
        std::string original;
        int index;

        inline std::string originalExt() const
        {
            auto splits = ut::strview(original).split(".");
            return splits.empty() ? "" : splits.back().str();
        }

        inline std::string originalName() const
        {
            auto sv = ut::strview(original);
            auto splits = sv.split(".");
            return splits.empty() ? original : sv.skipEnd(splits.back().size()+1).str();
        }
    };

    class Expression
    {
    public:
        Expression()=default;
        Expression(std::string const& expr);

        std::string toString(State const& state) const;

        inline bool empty() const
        { return m_vars.empty(); }

        inline varlist_type const& vars() const
        { return m_vars; }

    private:
        varlist_type m_vars;
    };
}
