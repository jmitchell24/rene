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
#include <string>


namespace rene
{
    struct RenameArgs
    {
        size_t index;
        std::string filename;
    };

    using renamer_type = ut::func<std::string(RenameArgs const&)>;


    class UserInterface
    {
    public:

        std::string const& expression() const;

        int run(renamer_type renamer);
        static UserInterface& instance();

    private:
        UserInterface();

        void updateRightList(renamer_type renamer);
    };


    [[maybe_unused]]
    static UserInterface& UI = UserInterface::instance();
}