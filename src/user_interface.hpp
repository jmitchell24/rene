//
// Created by james on 02/05/25.
//

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
    struct RenameArgs
    {
        size_t index;
        std::string filename;
    };

    using renamer_type = ut::func<std::string(RenameArgs const&)>;


    struct Replace
    {
        std::regex match;
        std::string replace;


    };

    using itemlist_type = std::vector<std::string>;
    using replacelist_type = std::vector<Replace>;



    class UserInterface
    {
    public:
        enum Mode { MODE_TEMPLATE, MODE_FIND_REPLACE };

        int run(renamer_type renamer);
        static UserInterface& instance();

    private:

        size_t          m_index_list;

        Replace         m_replace;
        itemlist_type   m_list_old;
        itemlist_type   m_list_new;

        std::string m_user_match;
        std::string m_user_match_error;

        Mode m_mode = MODE_FIND_REPLACE;

        UserInterface();

        void updateRightList();


    };


    [[maybe_unused]]
    static UserInterface& UI = UserInterface::instance();
}