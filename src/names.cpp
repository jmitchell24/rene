//
// Created by james on 18/05/25.
//

//
// rene
//
#include "names.hpp"
using namespace rene;

//
// std
//
#include <unordered_set>
using namespace std;

//
// NameList -> Implementation
//

bool NameList::isNewTextUnique() const
{
    unordered_set<string> tmp;
    for (auto&& it: names)
    {
        if (tmp.find(it.text_new) != tmp.end())
            return false;
        tmp.insert(it.text_new);
    }
    return true;
}

size_t NameList::maxExtentOld() const
{
    size_t cnt=0;
    for (auto&& it: names)
        if (cnt < it.text_old.size())
            cnt = it.text_old.size();
    return cnt;
}

bool NameList::isOldTextUnique() const
{
    unordered_set<string> tmp;
    for (auto&& it: names)
    {
        if (tmp.find(it.text_old) != tmp.end())
            return false;
        tmp.insert(it.text_old);
    }
    return true;
}

bool NameList::hasNewNames() const
{
    for (auto&& it: names)
    {
        if (it.text_new != it.text_old)
            return true;
    }
    return false;
}

pair<size_t, size_t> NameList::getExtents() const
{
    size_t cnt_new=0, cnt_old=0;

    for (auto&& it: names)
    {
        if (cnt_new < it.text_new.size())
            cnt_new = it.text_new.size();

        if (cnt_old < it.text_old.size())
            cnt_old = it.text_old.size();
    }

    return { cnt_new, cnt_old };
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
            names.push_back({it.path().filename().string(), ""});
    }

    this->names = move(names);
}