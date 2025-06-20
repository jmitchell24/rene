//
// Created by james on 02/05/25.
//
#pragma once

//
// rene
//
#include "fmt.hpp"
#include "names.hpp"
#include "user_interface_state.hpp"

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
#include <string>
#include <filesystem>



namespace rene
{
    using path_type = std::filesystem::path;

    class UserInterface
    {
    public:
        enum State { EDITING, ARMING };


        using path_type = std::filesystem::path;

        int run(path_type path);
        static UserInterface& instance();

    private:
        State m_state = EDITING;

        size_t m_highlighted_index=0;

        NameList m_names;

        std::string m_str_expression;

        ftxui::Element m_message;

        fmt::Expression m_expression;

        UserInterface();

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
