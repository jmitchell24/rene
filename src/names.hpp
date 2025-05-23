//
// Created by james on 18/05/25.
//

#pragma once

//
// ut
//
#include <ut/check.hpp>

//
// std
//

#include <string>
#include <vector>
#include <filesystem>

namespace rene
{
    struct Name;
    using names_type = std::vector<Name>;

    class Name
    {
    public:

        Name()=default;

        explicit Name(std::string text)
            : m_text_old(std::move(text))
        {}

        void setTextNew(std::string text)
        {
            m_text_new = std::move(text);
            m_generation++;
        }

        inline size_t generation() const
        { return m_generation; }

        inline std::string const& textNew() const
        { return m_text_new; }

        inline std::string const& textOld() const
        { return m_text_old; }

    private:
        std::string m_text_old="";
        std::string m_text_new="";
        size_t      m_generation=0;
    };

    struct NameList
    {
        using path_type = std::filesystem::path;
        using namelist_type = std::vector<Name>;

        namelist_type names;

        inline bool empty() const
        { return names.empty(); }

        inline size_t size() const
        { return names.size(); }

        size_t maxExtentOld() const;
        //size_t maxExtentOld() const;
        //size_t minExtentNew() const;
        //size_t minExtentOld() const;

        bool isOldTextUnique() const;
        bool isNewTextUnique() const;

        bool hasNewNames() const;
        std::pair<size_t, size_t> getExtents() const;

        inline Name& operator[] (size_t i)
        { check(i < names.size()); return names[i]; }

        inline Name const& operator[] (size_t i) const
        { return names[i]; }

        void loadFilenames(path_type const& path);
    };


}