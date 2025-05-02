
//
// rene
//
#include "user_interface.hpp"
using namespace rene;

//
// ut
//
#include <ut/ut.hpp>
using namespace ut;

//
// std
//
#include <iostream>
using namespace std;

auto g_help_text =(R"(
usage: rene [options] <directory>

Rene, bulk file renamer

Options:
    -h, --help       Display this help message and exit.
    -v, --verbose    Enable verbose output for detailed processing information.
    -n, --dry-run    Perform a dry run without renaming any files. Shows what would be renamed.
    -e, --extension <ext> Only rename files with the specified extension (default: all files).

Arguments:
    directory        The directory containing the files to rename.
    pattern          The pattern to match in the filenames for replacement.
    new_pattern      The new pattern to replace matched parts of the filenames.

Examples:
  rene /path/to/files oldname newname
    Rename all files in '/path/to/files' by replacing 'oldname' with 'newname'.

  rene --extension .txt /path/to/files text docx
    Rename all '.txt' files in '/path/to/files' by replacing 'text' with 'docx'.

  rene --dry-run /path/to/files old new
    Show what files would be renamed without performing the actual renaming.

For more information, visit: https://github.com/jmitchell24/rene
)"_sv).trim();

auto g_version_text = (R"(
rene, version 1.2.3
Copyright © 2025 James Peter Mitchell
For more information, visit: https://github.com/jmitchell24/rene
)"_sv).trim();

string renamer(RenameArgs const& ra)
{
    return to_string(ra.index) + " " + ra.filename;
}

int main(int argc, char** argv)
{
    return UI.run(fn_pointer(renamer));
}