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