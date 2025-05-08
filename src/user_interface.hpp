//
// Created by james on 02/05/25.
//


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
#include <regex>

namespace rene
{


    struct Name
    {
        std::string text_old;
        std::string text_new;
        //std::vector<std::smatch> matches;

        Name() { }
        Name(std::string const& s)
            : text_old(s), text_new()//, matches()
        { }


    };

    using names_type = std::vector<Name>;

    class UserInterface
    {
    public:
        enum Mode { MODE_TEMPLATE, MODE_FIND_REPLACE };

        int run();
        static UserInterface& instance();

    private:
        size_t m_highlighted_index=0;


        names_type m_names;



        std::string m_str_user_match;
        std::string m_str_user_replace;
        std::string m_str_error;

        //std::regex m_regex;
        Replacer m_replacer;

        UserInterface();

        void updateReplaceList();
        //void updateMatchList();

        ftxui::Elements createMatchElements();
        ftxui::Elements createReplaceElements();
    };
}
