//
// Created by james on 02/05/25.
//
#pragma once

//
// rene
//
#include "replacer.hpp"

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
    struct Name
    {
        std::string text_old;
        std::string text_new;
    };

    using names_type = std::vector<Name>;
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

        names_type m_names;

        std::string m_str_replace;
        std::string m_str_error;
        std::string m_str_warn;
        std::string m_str_info;

        Replacer m_replacer;

        UserInterface();

        void refreshNewNames();

        ftxui::Elements createOldNameElements();
        ftxui::Elements createNewNameElements();

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
