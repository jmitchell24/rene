//
// Created by james on 04/05/25.
//

//
// rene
//
#include "replacer.hpp"
using namespace rene;

//
// ut
//

//using namespace ut;

//
// std
//
#include <stdexcept>
using namespace std;



//
// Replacer -> Implementation
//

Replacer::Replacer(string const& expr)
    : m_expr{expr}
{ }

string Replacer::replace(string const& s) const
{
    if (m_expr.empty())
        return s;

    string result;

    size_t pos = 0;
    while (pos < s.size())
    {
        // Find opening delimiter
        auto open_pos = s.find("{{", pos);

        // If no more opening delimiters, append the rest of the string
        if (open_pos == string::npos)
        {
            result.append(s, pos, s.size() - pos);
            break;
        }

        // Look for closing delimiter
        auto close_pos = s.find("}}", open_pos + 2);
        if (close_pos == string::npos)
        {
            // Error: No closing delimiter
            throw runtime_error(
                "Missing closing delimiter '}}' for opening delimiter at position " + to_string(open_pos));
        }

        // Append text before the opening delimiter
        result += string(pos, open_pos - pos);

        // Extract variable name/expression and trim whitespace
        string var = s.substr(open_pos + 2, close_pos - (open_pos + 2));

        // Get the replacement value for this variable
        result += replaceVar(var);

        // Move past the closing delimiter
        pos = close_pos + 2;
    }

    return result;
}

// This function would be implemented to look up variable values
string Replacer::replaceVar(string const& var) const
{
    return "";
}