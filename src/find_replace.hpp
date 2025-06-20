//
// Created by james on 19/06/25.
//

#pragma once

//
// rene
//
#include "names.hpp"
#include "fmt.hpp"
#include "em.hpp"
#include "virtual_line_list.hpp"

//
// ut
//
#include <ut/string.hpp>

//
// std
//
#include <regex>

namespace rene
{
    class FindReplace
    {
    public:
        struct ReplaceResult
        {
            std::string replace_text;
            emlist_type replace_emlist;
            emlist_type find_emlist;
        };

        struct FindResult
        {
            emlist_type emlist;
        };

        FindResult getFindResult(ut::strparam s) const;
        ReplaceResult getReplaceResult(ut::strparam s) const;

        void setFind(ut::strparam s);
        void setReplace(ut::strparam s);

    //private:
        std::regex m_find;
        emlist_type m_find_emlist;

        fmt::Expression m_replace;
    };
}