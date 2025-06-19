//
// Created by james on 18/05/25.
//

#pragma once

//
// rene
//
#include "fmt.hpp"

//
// ut
//
#include <ut/string.hpp>
#include <ut/check.hpp>

//
// std
//

#include <string>
#include <vector>
#include <regex>
#include <filesystem>

namespace rene
{
    struct Name;
    using names_type = std::vector<Name>;

    /// \brief original name, immutable
    class OldName
    {
    public:
        /// \brief Emphasis Type
        enum EmType { SUB, MATCH, EXT };

        /// \brief section of text to emphasize
        struct Em { size_t begin=0, end=0; EmType type=SUB; };

        using emlist_type = std::vector<Em>;

        OldName(ut::strparam text)
            : m_text{text.str()}
        { }

        emlist_type getEmList() const;

    private:
        std::string m_text;
    };

    /// \brief modified name, mutable (built in place)
    class NewName
    {
    public:
        /// \brief Emphasis Type
        enum SubType { SUB, MATCH, VAR };

        /// \brief section of text to emphasize
        struct Sub { size_t begin=0, end=0; SubType type=SUB; };


        using sublist_type = std::vector<Sub>;

        NewName() {}



        void append(ut::strparam text, SubType);

    private:
        std::string m_text;
        sublist_type m_sublist;
    };



    class Name
    {
    public:
        enum EmType { EM_NONE, EM_MATCH, EM_EXT, EM_VAR };
        struct Sub { size_t begin, end; EmType type=EM_NONE; };
        using sublist_type = std::vector<Sub>;

        Name()=default;

        explicit Name(ut::strparam text);

        void setTextNew(ut::strparam text);

        sublist_type getSubsOld(std::regex const& r) const;

        inline size_t generation() const
        { return m_generation; }

        inline std::string const& textNew() const
        { return m_text_new; }

        inline std::string const& textOld() const
        { return m_text_old; }

        std::string getNewName(fmt::Expression const& expression) const;

    private:
        std::string m_text_old="";
        std::string m_text_new="";

        size_t m_generation=0;
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
        void loadDummies(size_t count);
        void loadNewNames(std::vector<std::string> const& new_names);
    };


}