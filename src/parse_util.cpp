//
// Created by james on 10/05/25.
//

//
// rene
//
#include "parse_util.hpp"
using namespace rene;

//
// ut
//
// using namespace ut;

//
// std
//
#include <stdexcept>
using namespace std;

arglist_type rene::parseArgList(string const& input)
{
    arglist_type args;
    string current_arg;

    enum QuoteState { QS_NONE, QS_SINGLE, QS_DOUBLE };

    QuoteState quote_state = QS_NONE;
    bool escaped = false;

    for (auto&& it: input)
    {
        // Handle escape character
        if (escaped)
        {
            // In double quotes, only certain characters are treated specially when escaped
            if (quote_state == QS_DOUBLE)
            {
                // In double quotes, \ only escapes $, `, ", \, and newline
                if (it == '$' || it == '`' || it == '"' || it == '\\' || it == '\n')
                {
                    current_arg += it;
                }
                else
                {
                    // Otherwise, both the backslash and the character are preserved
                    current_arg += '\\';
                    current_arg += it;
                }
            }
            else
            {
                // Outside quotes or in single quotes, \ escapes any character
                current_arg += it;
            }
            escaped = false;
            continue;
        }

        // Check for escape character
        if (it == '\\' && quote_state != QS_SINGLE)
        {
            escaped = true;
            continue;
        }

        // Handle quotes
             if (it == '\'' && quote_state == QS_NONE)   { quote_state = QS_SINGLE; continue; }
        else if (it == '\'' && quote_state == QS_SINGLE) { quote_state = QS_NONE;   continue; }
        else if (it == '"'  && quote_state == QS_NONE)   { quote_state = QS_DOUBLE; continue; }
        else if (it == '"'  && quote_state == QS_DOUBLE) { quote_state = QS_NONE;   continue; }

        // Handle whitespace outside of quotes
        if (isspace(it) && quote_state == QS_NONE)
        {
            if (!current_arg.empty())
            {
                args.push_back(current_arg);
                current_arg.clear();
            }
            continue;
        }

        // Add character to current token
        current_arg += it;
    }

    // Handle any remaining escaped character at the end
    if (escaped)
        current_arg += '\\';

    // Add the last token if not empty
    if (!current_arg.empty())
        args.push_back(current_arg);


    // Error state: unclosed quotes (bash would prompt for more input)
    if (quote_state != QS_NONE)
        throw runtime_error("unclosed quotes");

    return args;
}

#define INSERT_FIELD(x_) fields.push_back({ TextField::FIELD, (x_) })
#define INSERT_TEXT(x_) fields.push_back({ TextField::TEXT, (x_) })

fieldlist_type rene::parseFields(string const& input, char ch_open, char ch_close)
{
    fieldlist_type fields;
    string current_text;

    size_t i = 0;

    while (i < input.size())
    {
        char it = input[i];

        // Check for opening bracket
        if (it == ch_open)
        {
            // Check if it's a double bracket (escaped)
            if (i + 1 < input.size() && input[i + 1] == ch_open)
            {
                // Add the current text buffer (if any) to tokens
                if (!current_text.empty())
                {
                    INSERT_TEXT(current_text);
                    current_text.clear();
                }

                // Add a single '[' to the current text
                current_text += ch_open;
                i += 2; // Skip both brackets
            }
            else
            {
                // This is the start of a format specifier

                // Add the current text buffer (if any) to tokens
                if (!current_text.empty())
                {
                    INSERT_TEXT(current_text);
                    current_text.clear();
                }

                // Find the closing bracket
                size_t closing_bracket = i + 1;
                bool found_closing = false;

                while (closing_bracket < input.size())
                {
                    if (input[closing_bracket] == ch_close)
                    {
                        // Check if it's an escaped closing bracket
                        if (closing_bracket + 1 < input.size() && input[closing_bracket + 1] == ch_close)
                        {
                            closing_bracket += 2; // Skip both brackets
                        }
                        else
                        {
                            // Found the unescaped closing bracket
                            found_closing = true;
                            break;
                        }
                    } else {
                        // Continue searching
                        closing_bracket++;
                    }
                }

                if (!found_closing)
                {
                    throw runtime_error("Unmatched opening bracket at position " + to_string(i));
                }

                // Extract the format specifier content (without the brackets)
                auto format_content = input.substr(i + 1, closing_bracket - i - 1);

                // Process the format content to handle escaped brackets
                string processed_content;
                for (size_t j = 0; j < format_content.size(); j++)
                {
                    if (format_content[j] == ch_close && j + 1 < format_content.size() && format_content[j + 1] == ch_close)
                    {
                        processed_content += ch_close;
                        j++; // Skip the second bracket
                    }
                    else if (format_content[j] == ch_open && j + 1 < format_content.size() && format_content[j + 1] == ch_open)
                    {
                        processed_content += ch_open;
                        j++; // Skip the second bracket
                    }
                    else
                    {
                        processed_content += format_content[j];
                    }
                }

                INSERT_FIELD(processed_content);
                i = closing_bracket + 1; // Move past the closing bracket
            }
        }
        else if (it == ch_close)
        {
            // Check if it's a double bracket (escaped)
            if (i + 1 < input.size() && input[i + 1] == ch_close)
            {
                // Add the current text buffer (if any) to tokens
                if (!current_text.empty())
                {
                    INSERT_TEXT(current_text);
                    current_text.clear();
                }

                // Add a single ']' to the current text
                current_text += ch_close;
                i += 2; // Skip both brackets
            }
            else
            {
                // Unmatched closing bracket is an error
                throw runtime_error("Unmatched closing bracket at position " + to_string(i));
            }
        }
        else
        {
            // Regular character, add to current text buffer
            current_text += it;
            i++;
        }
    }

    // Add any remaining text
    if (!current_text.empty())
    {
        INSERT_TEXT(current_text);
    }

    return fields;
}

#undef INSERT_FIELD
#undef INSERT_TEXT