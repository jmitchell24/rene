//
// Created by james on 04/05/25.
//

#pragma once

#include <string>
#include <variant>

namespace rene
{
    // struct VarOriginalName { };
    //
    // struct Var
    // {
    //     enum Kind { EMPTY, ORIGINAL_NAME };
    //     using var_type = std::variant<
    //         std::monostate,
    //         VarOriginalName>;
    //
    //     inline bool empty() const { return data.index()==0; }
    //     inline bool isOriginalName() const { return data.index()==1; }
    //
    //     var_type data;
    // };

    class Replacer
    {
    public:
        Replacer(std::string const& expr);

        std::string replace(std::string const& s) const;

    private:
        std::string m_expr;
        std::string replaceVar(std::string const& var) const;
    };
}