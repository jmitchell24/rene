//
// Created by james on 02/05/25.
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
#include <ut/func.hpp>

//
// std
//
#include <vector>
#include <string>
#include <filesystem>

#define EXPAND_STATES(_state) \
    _var(StateEditing, EDITING) \
    _var(StateArming, ARMING) \
    _var(StateError, ERROR) \
    _var(VarFuzz, FUZZ)

#define STATE_ENUM(_x, _y) ,_y
#define STATE_VARIANT(_x, _y) ,_x

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
        ftxui::Elements m_old_names;
        ftxui::Elements m_new_names;

        fmt::Expression m_expression;

        UserInterface();

        void refreshNewNames();
        void refreshOldNames();

        ftxui::Elements createOldNameElements();
        ftxui::Elements createNewNameElements();
        ftxui::Element createOldNameElement(int line_num, Name const& name, bool is_selected);
        ftxui::Element createNewNameElement(int line_num, Name const& name, bool is_selected);

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
