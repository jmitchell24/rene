//
// Created by james on 19/06/25.
//

#include "find_replace.hpp"
using namespace rene;

//
// ut
//
using namespace ut;

//
// std
//
using namespace std;

//
// FindReplace - Impl
//

FindReplace::ReplaceResult FindReplace::getReplaceResult(strparam s, State const& state) const
{
    auto rb = cregex_iterator(s.begin(), s.end(), m_find);
    auto re = cregex_iterator();

    emlist_type find_emlist;
    fmt::matchlist_type matches;

    size_t last=0;

    for (auto it = rb; it != re; ++it)
    {
        size_t ib = it->position();
        size_t ie = ib + it->length();

        if (ib > last)
            find_emlist.push_back({Em::NONE, last, ib});
        find_emlist.push_back({Em::MATCH, ib, ie});
        matches.push_back({ib, ie});

        last = ie;
    }

    if (size_t end = s.size(); last < end)
        find_emlist.push_back({Em::NONE, last, end});

    fmt::Expression::State fmt_state;
    fmt_state.matches = std::move(matches);
    fmt_state.index = state.index;
    fmt_state.total = state.total;
    fmt_state.original = s.str();

    auto res = m_replace.getResult(fmt_state);

    return { res.text, res.emlist, find_emlist };
}

FindReplace::FindResult FindReplace::getFindResult(strparam s) const
{
    auto rb = cregex_iterator(s.begin(), s.end(), m_find);
    auto re = cregex_iterator();

    emlist_type find_emlist;

    size_t last=0;

    for (auto it = rb; it != re; ++it)
    {
        size_t ib = it->position();
        size_t ie = ib + it->length();

        if (ib > last)
            find_emlist.push_back({Em::NONE, last, ib});
        find_emlist.push_back({Em::MATCH, ib, ie});

        last = ie;
    }

    if (size_t end = s.size(); last < end)
        find_emlist.push_back({Em::NONE, last, end});

    return { find_emlist };
}

void FindReplace::setFind(strparam s)
{
    try { m_find = regex(s.begin(), s.size()); } catch (...) {}
}

void FindReplace::setReplace(strparam s)
{
    try { m_replace = fmt::Expression(s); } catch (...) {}
}

