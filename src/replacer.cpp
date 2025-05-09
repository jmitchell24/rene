//
// Created by james on 04/05/25.
//

//
// rene
//
#include "replacer.hpp"

#include "ut/string/view.hpp"
using namespace rene;

//
// ut
//
#include <ut/check.hpp>
#include <ut/random.hpp>
using namespace ut;

//
// std
//
#include <stdexcept>
#include <expected>
using namespace std;

//
// Parser Helper
//

struct Tok
{
    enum Kind { TEXT, SPEC };

    Kind kind;
    string text;

    inline bool isText() const { return kind == TEXT; }
    inline bool isSpec() const { return kind == SPEC; }
};

using toklist_type = vector<Tok>;

toklist_type parseSpecifiers(string const& input, char ch_open = '[', char ch_close = ']')
{
    toklist_type toks;
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
                    toks.push_back({ Tok::TEXT, current_text });
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
                    toks.push_back({ Tok::TEXT, current_text });
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

                toks.push_back({Tok::SPEC, processed_content});
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
                    toks.push_back({Tok::TEXT, current_text});
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
        toks.push_back({Tok::TEXT, current_text});
    }

    return toks;
}


Var tokToVar(Tok const& tok)
{
    if (tok.isText())
        return { VarLiteral{tok.text} };

    auto s = strview(tok.text).trim();

    if (s.empty())
        return { VarOriginal{} };

    if (s == "orig"_sv)
        return { VarOriginal{} };

    if (s == "original"_sv)
        return { VarOriginal{} };

    if (s == "inc"_sv)
        return { VarIncrement{} };

    if (s == "increment"_sv)
        return { VarIncrement{} };

    if (s == "fuzz"_sv)
        return { VarFuzz{} };

    throw runtime_error("invalid format specifier: '" + tok.text + "'");
    return {};
}

//
// Replacer -> Implementation
//

Replacer::Replacer(string const& expr)
{
    auto toks = parseSpecifiers(expr);
    for (auto&& it: toks)
        m_vars.push_back(tokToVar(it));
}

std::string getRandomFakeWord(int i)
{
    static std::vector<std::string> random_words = {
        "acorn", "barnacle", "cactus", "dandelion", "eagle",
        "falcon", "grape", "hedgehog", "iguana", "jaguar",
        "kangaroo", "lemur", "mongoose", "nematode", "octopus",
        "penguin", "quokka", "rhinoceros", "salamander", "toucan",
        "urchin", "vulture", "wombat", "xenodochial", "yeti",
        "zebra", "abruptly", "bandwagon", "cemetery", "dizzying",
        "fjord", "gossip", "hymn", "jazzy", "kiosk",
        "lumberjack", "mystify", "numbats", "ovary", "pajama",
        "quixotic", "rhythm", "syzygy", "turbulent", "unbelievable",
        "vexillology", "wistful", "xylophone", "yachtsman", "zephyr",
        // Add more words as needed to fill up the array
    };

    static size_t off = ut_rng.nextu(random_words.size());

    return random_words[(off + i) % random_words.size()];
}

string Replacer::replace(Args const& args) const
{
    string res;

    for (auto&& it: m_vars)
    {
        switch (it.kind())
        {
            case Var::EMPTY: break;
            case Var::LITERAL: res += it.asLiteral().text; break;
            case Var::ORIGINAL: res += args.original; break;
            case Var::INCREMENT: res += to_string(args.increment); break;
            case Var::FUZZ: res += getRandomFakeWord(args.increment); break;
            default:nopath_case(Var::Kind);
        }
    }

    return res;
}