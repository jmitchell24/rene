//
// Created by james on 13/06/25.
//

#pragma once


//
// rene
//

#include "fmt.hpp"
#include "names.hpp"

//
// ftxui
//

#include "ftxui/dom/elements.hpp"

//
// ut
//

//
// std
//

namespace rene
{

    class UserInterfaceSed
    {
    public:
        enum State { EDITING, ARMING };


        using path_type = std::filesystem::path;

        int run(NameList const& names);
        static UserInterfaceSed& instance();

    private:
        State m_state = EDITING;

        size_t m_highlighted_index=0;

        NameList m_names;

        std::string m_str_match;
        std::string m_str_expression;

        ftxui::Element m_message;


        std::regex m_regex;
        fmt::Expression m_expression;

        UserInterfaceSed();

        void refreshNewNames();
        void refreshOldNames();

        void updateHighlightedIndex(size_t new_idx);

        void changeEditing();
        void changeArming();

        inline bool isEditing() const
        { return m_state == EDITING; }

        inline bool isArming() const
        { return m_state == ARMING; }

        void setInfo(std::string const& s);
        void setWarn(std::string const& s);
        void setError(std::string const& s);

        
    };
}