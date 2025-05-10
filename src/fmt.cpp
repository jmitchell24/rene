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
#include <expected>
using namespace std;

//
// Parser Helper
//



VarIncrement varIncrement(bool is_inc, arglist_type const& args)
{
    if (args.size() == 1)
    {
        return VarIncrement{ .offset=0 };
    }

    if (args.size() == 2)
    {
        int offset;
        auto&& a = args[1];
        from_chars(a.data(), a.data()+a.size(), offset);
        return VarIncrement{ .offset=offset };
    }

    throw runtime_error("invalid args for increment");
    return { };
}

Var fieldToVar(TextField const& tf)
{
    if (tf.isText())
        return { VarLiteral{tf.text} };

    auto args = parseArgList(tf.text);

    if (args.empty())
        return { VarOriginal{} };

    if (args[0] == "orig")
        return { VarOriginal{} };

    if (args[0] == "original")
        return { VarOriginal{} };

    if (args[0] == "inc")
        return { varIncrement(true, args) };

    if (args[0] == "increment")
        return { varIncrement(true, args) };

    if (args[0] == "fuzz")
        return { VarFuzz{} };

    throw runtime_error("invalid format specifier: '" + tf.text + "'");
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
            case Var::INCREMENT: oss << to_string(state.increment + it.asIncrement().offset); break;
            case Var::FUZZ: oss << getRandomFakeWord(state.increment); break;
            default:nopath_case(Var::Kind);
        }
    }

    return oss.str();
}