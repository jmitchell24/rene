
//
// rene
//
#include <unistd.h>

#include "cmd.hpp"
#include "rene.hpp"
#include "user_interface.hpp"
#include "user_interface_sed.hpp"
using namespace rene;

//
// ut
//
#include <ut/ut.hpp>
#include <ut/term.hpp>
using namespace ut;

//
// std
//
#include <iostream>
using namespace std;

#include <string>


string makeTestFilename()
{
    // Generate a unique test filename
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dis(1000, 9999);

    // Get current timestamp for uniqueness
    auto now = chrono::system_clock::now();
    auto timestamp = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();

    stringstream filename;
    filename << "test_file_" << timestamp << "_" << dis(gen) << ".txt";

    return filename.str();
}

void makeTestFiles(filesystem::path path, size_t count) {
    // Create the directory if it doesn't exist
    filesystem::create_directories(path);

    // Delete existing test files (files matching test_file_*.txt pattern)
    for (const auto& entry : filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            string filename = entry.path().filename().string();
            if (filename.starts_with("test_file_") && filename.ends_with(".txt")) {
                filesystem::remove(entry.path());
            }
        }
    }

    // Generate different types of test content
    vector<string> contentTypes = {
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
        "Test data with numbers: 123, 456, 789",
        "Multi-line\ntest\ncontent\nwith\nnewlines",
        "Special characters: !@#$%^&*()_+-=[]{}|;':\",./<>?",
        "JSON-like: {\"key\": \"value\", \"number\": 42, \"array\": [1,2,3]}"
    };

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> contentDis(0, contentTypes.size() - 1);
    uniform_int_distribution<> sizeDis(1, 5); // Number of content blocks per file

    for (size_t i = 0; i < count; ++i) {
        string filename = makeTestFilename();
        filesystem::path filepath = path / filename;

        ofstream file(filepath);
        if (file.is_open()) {
            // Write file header
            file << "# Test File " << (i + 1) << " of " << count << "\n";
            file << "# Generated: " << chrono::system_clock::now().time_since_epoch().count() << "\n\n";

            // Write random content
            int blocks = sizeDis(gen);
            for (int j = 0; j < blocks; ++j) {
                file << contentTypes[contentDis(gen)] << "\n";
                if (j < blocks - 1) file << "\n"; // Add spacing between blocks
            }

            file.close();
        }
    }
}

int main(int argc, char** argv)
{
    CmdArgs args{argc, argv};

    if (args.flagHelp())
    {
        cout << RENE_HELP_TEXT << endl;
        return EXIT_SUCCESS;
    }

    if (args.flagVersion())
    {
        cout << RENE_VERSION_TEXT << endl;
        return EXIT_SUCCESS;
    }



    // if (args.flagSedMode())
    // {
    //
    //     NameList nl;
    //     nl.loadDummies(100);
    //     return UserInterfaceSed::instance().run(nl);
    // }



    array<char, 1000> cwdbuf;
    getcwd(cwdbuf.data(), cwdbuf.size());
    filesystem::path path = cwdbuf.data();

    if (string arg; args.tryGetPositional(0, arg))
    {
        path = path_type(arg);

        if (!filesystem::exists(path))
        {
            cout << path << ": " TERM_FG_BRIGHT_RED "No such directory" TERM_RESET "\n";
            return EXIT_FAILURE;
        }
    }

    if (args.flag("make_test_files"))
    {
        makeTestFiles(path, 10);
    }

    return UserInterface::instance().run(path);
}

