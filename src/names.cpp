//
// Created by james on 18/05/25.
//

//
// rene
//
#include "names.hpp"
using namespace rene;

//
// ut
//
#include <ut/random.hpp>
using namespace ut;

//
// std
//
#include <unordered_set>
using namespace std;

//
// Name Implementation
//

Name::Name(strparam text)
    : m_text_old(text.str()), m_text_new(text.str())
{ }

void Name::setTextNew(strparam text)
{
    m_text_new = text.str();
    m_generation++;
}

Name::sublist_type Name::getSubsOld(regex const& r) const
{
    auto reg_begin = sregex_iterator(m_text_old.begin(), m_text_old.end(), r);
    auto reg_end = sregex_iterator();

    sublist_type subs;

    size_t last=0;

    for (auto it = reg_begin; it != reg_end; ++it)
    {
        size_t ibeg = it->position();
        size_t iend = ibeg + it->length();

        if (ibeg > last)
            subs.push_back({last, ibeg, false});
        subs.push_back({ibeg, iend, true});

        last = iend;
    }

    if (size_t end = m_text_old.size(); last < end)
        subs.push_back({last, end, false});

    return subs;
}


//
// NameList Implementation
//

bool NameList::isNewTextUnique() const
{
    unordered_set<string> tmp;
    for (auto&& it: names)
    {
        if (tmp.find(it.textNew()) != tmp.end())
            return false;
        tmp.insert(it.textNew());
    }
    return true;
}

size_t NameList::maxExtentOld() const
{
    size_t cnt=0;
    for (auto&& it: names)
        if (cnt < it.textOld().size())
            cnt = it.textOld().size();
    return cnt;
}

bool NameList::isOldTextUnique() const
{
    unordered_set<string> tmp;
    for (auto&& it: names)
    {
        if (tmp.find(it.textOld()) != tmp.end())
            return false;
        tmp.insert(it.textOld());
    }
    return true;
}

bool NameList::hasNewNames() const
{
    for (auto&& it: names)
    {
        if (it.textNew() != it.textOld())
            return true;
    }
    return false;
}

pair<size_t, size_t> NameList::getExtents() const
{
    size_t cnt_new=0, cnt_old=0;

    for (auto&& it: names)
    {
        if (cnt_new < it.textNew().size())
            cnt_new = it.textNew().size();

        if (cnt_old < it.textOld().size())
            cnt_old = it.textOld().size();
    }

    return { cnt_new, cnt_old };
}

string createDummy()
{
    // Common consonants and vowels for realistic-sounding words
    vector<string> consonants = {
        "b", "c", "d", "f", "g", "h", "j", "k", "l", "m",
        "n", "p", "q", "r", "s", "t", "v", "w", "x", "z",
        "ch", "sh", "th", "ph", "bl", "br", "cl", "cr",
        "dr", "fl", "fr", "gl", "gr", "pl", "pr", "sl",
        "sm", "sn", "sp", "st", "sw", "tr"
    };

    vector<string> vowels = {
        "a", "e", "i", "o", "u", "y",
        "ai", "au", "ea", "ee", "ei", "ie", "oa", "oo", "ou"
    };

    vector<string> endings = {
        "ed", "er", "est", "ing", "ion", "ly", "ness",
        "ful", "less", "ment", "able", "ible", "ous", "al"
    };

    string word = "";
    int syllables = ut_rng.nexti(2, 4); // 2-4 syllables

    // Build syllables (consonant-vowel pattern)
    for (int i = 0; i < syllables; i++) {
        // Add consonant (skip for first syllable sometimes)
        if (i > 0 || ut_rng.nextb()) {
            word += ut_rng.choose(consonants);
        }
        // Add vowel
        word += ut_rng.choose(vowels);
    }

    // Occasionally add an ending (1 in 3 chance)
    if (ut_rng.nexti(3) == 0) {
        word += ut_rng.choose(endings);
    }

    // Ensure minimum length
    if (word.length() < 3) {
        word += ut_rng.choose(vowels);
    }

    return word;
}

void NameList::loadDummies(size_t count)
{
    names.clear();
    for (size_t i = 0; i < count; ++i)
        names.push_back(Name(createDummy()));
}

void NameList::loadFilenames(path_type const& path)
{
    using namespace std::filesystem;
    names_type names;

    if (!exists(path))
        return;

    if (!is_directory(path))
        return;

    for (auto&& it : directory_iterator(path))
    {
        if (it.is_regular_file())
            names.push_back(Name(it.path().filename().string()));
    }

    this->names = move(names);
}

void NameList::loadNewNames(std::vector<std::string> const& new_names)
{

}

#pragma endregion NameList
