//
// Created by james on 04/05/25.
//

//
// rene
//
#include "fmt.hpp"
#include "parse_util.hpp"
using namespace rene;
using namespace rene::fmt;

//
// ut
//
#include <ut/check.hpp>
#include <ut/random.hpp>
#include <ut/string.hpp>
using namespace ut;

//
// std
//
#include <stdexcept>
using namespace std;

//
// Parser Helper
//



VarIndex varIndex(arglist_type const& args)
{
    if (args.size() == 1)
    {
        return VarIndex{ .offset=0 };
    }

    if (args.size() == 2)
    {
        int offset;
        auto&& a = args[1];
        from_chars(a.data(), a.data()+a.size(), offset);
        return VarIndex{ .offset=offset };
    }

    throw runtime_error("invalid args for increment");
    return { };
}

VarMatch varMatch(arglist_type const& args)
{
    if (args.size() == 1)
    {
        return VarMatch{ .index = 0 };
    }

    if (args.size() == 0)
    {
        int index;
        auto&& a = args[1];
        from_chars(a.data(), a.data()+a.size(), index);
        return VarMatch{ .index=index };
    }
    throw runtime_error("invalid args for index");
    return { };
}

Var fieldToVar(TextField const& tf)
{
    if (tf.isText())
        return { VarLiteral{tf.text} };

    auto args = parseArgList(tf.text);

    if (args.empty())
        return { VarOriginal{} };

    if (args[0] == "name")
        return { VarOriginalName{} };

    if (args[0] == "ext")
        return { VarOriginalExt{} };

    if (args[0] == "index")
        return { varIndex(args) };

    if (args[0] == "match")
        return { varMatch(args) };

    if (args[0] == "fuzz")
        return { VarFuzz{} };

    throw runtime_error("invalid template: '" + tf.text + "'");
    return {};
}

//
// Replacer -> Implementation
//

Expression::Expression(string const& expr)
{
    auto fields = parseFields(expr);
    for (auto&& it: fields)
        m_vars.push_back(fieldToVar(it));
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
    };

    static size_t off = ut_rng.nextu(random_words.size());

    return random_words[(off + i) % random_words.size()];
}

string Expression::toString(State const& state) const
{
    ostringstream oss;

    for (auto&& it: m_vars)
    {
        switch (it.kind())
        {
            case Var::EMPTY: break;
            case Var::LITERAL: oss << it.asLiteral().text; break;
            case Var::ORIGINAL: oss << state.original; break;
            case Var::ORIGINAL_EXT: oss << state.originalExt(); break;
            case Var::ORIGINAL_NAME: oss << state.originalName(); break;
            case Var::INDEX: oss << to_string(state.index + it.asIndex().offset); break;
            case Var::FUZZ: oss << getRandomFakeWord(state.index); break;


        case Var::MATCH:
            if (size_t i = it.asMatch().index; i < state.matches.size())
                oss << state.matches[i];
            break;

            default:nopath_case(Var::Kind);
        }
    }

    return oss.str();
}