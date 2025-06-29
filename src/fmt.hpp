//
// Created by james on 04/05/25.
//

#pragma once

//
// rene
//
#include "em.hpp"

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
    _var(Match, MATCH) \
    _var(Fuzz, FUZZ) \
    _var(Total, TOTAL)

#define VAR_ENUM(_x, _y)        ,_y
#define VAR_VARIANT(_x, _y)     ,Var##_x
#define VAR_GETTER(_x, _y)      inline Var##_x const& as##_x() const { check(kind() == _y); return std::get<_y>(data); }

namespace rene::fmt
{
    struct Var;
    struct VarLiteral       { std::string text; };
    struct VarOriginal      { };
    struct VarOriginalName  { };
    struct VarOriginalExt   { };
    struct VarIndex         { int offset; };
    struct VarMatch         { int index; };
    struct VarFuzz          { };
    struct VarTotal         { };

    using varlist_type = std::vector<Var>;

    struct Var
    {
        enum Kind{ EMPTY EXPAND_VARS(VAR_ENUM) };
        using var_type = std::variant<std::monostate EXPAND_VARS(VAR_VARIANT)>;
        EXPAND_VARS(VAR_GETTER)

        inline Kind kind() const { return static_cast<Kind>(data.index()); }
        var_type data;
    };

    using matchlist_type = std::vector<ut::strview::indices>;



    class Expression
    {
    public:
        /// \brief vars required by getResult()
        struct State
        {

            /// \brief unmodified input text
            std::string original;

            /// \brief matched substrings of input text (if any)
            matchlist_type matches;

            /// \brief index value of the input text
            int index;

            /// \brief total number of input texts
            int total;

            /// \brief input text after final '.' char (if any)
            inline std::string originalExt() const
            {
                auto splits = ut::strview(original).split(".");
                return splits.empty() ? "" : splits.back().str();
            }

            /// \brief input text before final '.' char (if any)
            inline std::string originalName() const
            {
                auto sv = ut::strview(original);
                auto splits = sv.split(".");
                return splits.empty() ? original : sv.skipEnd(splits.back().size()+1).str();
            }
        };

        struct Result
        {
            std::string text;
            emlist_type emlist;
        };

        Expression()=default;
        Expression(ut::strparam s);

        Result getResult(State const& state) const;

        inline bool empty() const
        { return m_vars.empty(); }

        inline varlist_type const& vars() const
        { return m_vars; }

    private:
        varlist_type m_vars;
    };
}
