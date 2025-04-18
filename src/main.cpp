#include <ut/string.hpp>
using namespace ut;

#include <iostream>
using namespace std;

int main()
{
    auto sv = "Hello, World"_sv;
    cout << sv << endl;
    return EXIT_SUCCESS;
}
