
#include "cmd.hpp"
using namespace rene;

#include <ut/string.hpp>
#include <ut/term.hpp>
using namespace ut;


#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <variant>
using namespace std;



int main(int argc, char** argv)
{
    CmdArgs cargs;
    cargs.parse(argc, argv);






    cargs.dbgPrint();


    // auto sv = "Rene, the terminal based bulk renamer utliity. "_sv;
    // cout << sv << endl;



    return EXIT_SUCCESS;
}
