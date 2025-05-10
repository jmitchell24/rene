//
// Created by james on 10/05/25.
//

#pragma once

//
// std
//
#include <string>
#include <vector>

namespace rene
{
    /// represents a list of strings (e.g. "arg1 arg2 'arg3 with spaces'") 
    using arglist_type = std::vector<std::string>;

    /// "bourne shell"-style argument list parser (including correct quote handling) 
    /// @param input the string to be parsed
    /// @return a vector of strings representing the parsed arguments
    arglist_type parseArgList(std::string const& input);

    struct TextField
    {
        enum Kind { TEXT, FIELD } kind=TEXT;
        std::string text;
        inline bool isText() const { return kind == TEXT; }
        inline bool isField() const { return kind == FIELD; }
    };

    /// represents a list of text and fields (e.g. "[field1] some text [field2]")
    using fieldlist_type = std::vector<TextField>;

    /// parses a string into text and fields (e.g. "[field1] some text [field2]")
    /// @param input the string to be parsed
    /// @param ch_open the character that opens a field
    /// @param ch_close the character that closes a field
    /// @return a vector of TextField structs representing the parsed text and fields
    fieldlist_type parseFields(std::string const& input, char ch_open = '[', char ch_close = ']');
}