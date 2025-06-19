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

Expression::Expression(strparam s)
{
    auto fields = parseFields(s.str());
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

Expression::Result Expression::getResult(State const& state) const
{
    Result res;

#define APPEND(_s, _em) { \
    string s = _s; \
    res.text += s; \
    res.emlist.push_back({_em, res.text.size() - s.size(), res.text.size()}); \
    }

    for (auto&& it: m_vars)
    {
        switch (it.kind())
        {
            case Var::EMPTY: break;
            case Var::LITERAL: APPEND(it.asLiteral().text, Em::NONE); break;
            case Var::ORIGINAL: APPEND(state.original, Em::VAR); break;
            case Var::ORIGINAL_EXT: APPEND(state.originalExt(), Em::NONE); break;
            case Var::ORIGINAL_NAME: APPEND(state.originalName(), Em::NONE); break;
            case Var::INDEX: APPEND(to_string(state.index + it.asIndex().offset), Em::NONE); break;
            case Var::FUZZ: APPEND(getRandomFakeWord(state.index), Em::NONE); break;


        case Var::MATCH:
            if (size_t i = it.asMatch().index; i < state.matches.size())
            {
                APPEND(state.matches[i].view(state.original).str(), Em::NONE);
            }
            else
            {
                APPEND("???", Em::NONE);
            }
            break;

            default:nopath_case(Var::Kind);
        }
    }

#undef APPEND

    return res;
}