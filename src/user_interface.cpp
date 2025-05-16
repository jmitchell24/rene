//
// Created by james on 02/05/25.
//

//
// rene
//
#include "user_interface.hpp"
#include "fmt.hpp"
#include "rene.hpp"
using namespace rene;

//
// ftxui
//
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component_options.hpp>
#include "ftxui_extensions.hpp"
using namespace ftxui;

//
// ut
//
#include <ut/random.hpp>
using namespace ut;

//
// std
//
#include <regex>
#include <cstdlib>
#include <unordered_set>
using namespace std;

//
// NameList Implementation
//



bool NameList::isNewTextUnique() const
{
    unordered_set<string> tmp;
    for (auto&& it: names)
    {
        if (tmp.find(it.text_new) != tmp.end())
            return false;
        tmp.insert(it.text_new);
    }
    return true;
}

bool NameList::isOldTextUnique() const
{
    unordered_set<string> tmp;
    for (auto&& it: names)
    {
        if (tmp.find(it.text_old) != tmp.end())
            return false;
        tmp.insert(it.text_old);
    }
    return true;
}

bool NameList::hasNewNames() const
{
    for (auto&& it: names)
    {
        if (it.text_new != it.text_old)
            return true;
    }
    return false;
}

pair<size_t, size_t> NameList::getExtents() const
{
    size_t cnt_new=0, cnt_old=0;

    for (auto&& it: names)
    {
        if (cnt_new < it.text_new.size())
            cnt_new = it.text_new.size();

        if (cnt_old < it.text_old.size())
            cnt_old = it.text_old.size();
    }

    return { cnt_new, cnt_old };
}

void NameList::loadFilenames(path_type const& path)
{
    using namespace std::filesystem;
    names_type names;

    if (!exists(path))
        return;

    if (!is_directory(path))
        return;

    for (auto&& it : directory_iterator(path))
    {
        if (it.is_regular_file())
            names.push_back({it.path().filename().string(), ""});
    }

    this->names = move(names);
}

void renameFile(path_type path, string const& old_name, string const& new_name)
{
    using namespace std::filesystem;

    if (!exists(path))
        return;

    if (!is_directory(path))
        return;

    auto old_path = path / old_name;
    auto new_path = path / new_name;

    if (!exists(old_path))
        return;

    check(!exists(new_path));
    rename(old_path, new_path);
}

void renameAllFiles(path_type path, NameList const& names)
{
    using namespace std::filesystem;

    if (!exists(path))
        return;

    if (false)
    {
        for (auto&& it: names.names)
        {
            renameFile(path, it.text_old, it.text_new);
        }
    }
    else
    {
        for (auto&& it: names.names)
        {
            renameFile(path, it.text_old, it.text_new + ".rene");
        }

        for (auto&& it: names.names)
        {
            renameFile(path, it.text_new + ".rene", it.text_new);
        }
    }



}


//
// UserInterface -> Implementation
//


UserInterface::UserInterface()
{
    setInfo("test");
}

Elements UserInterface::createNewNameElements()
{
    Elements el;

    for (size_t i = 0; i < m_names.size(); ++i)
    {


        auto&& it = m_names[i];
        bool is_selected = (i == m_highlighted_index);
        bool is_empty = it.text_new.empty();

        if (is_selected)
        {
            el.push_back(text(" " + it.text_new) | inverted | focus);
        }
        else
        {
            el.push_back(text(" " + it.text_new));
        }

    }
    return el;
}


Elements UserInterface::createOldNameElements()
{
    Elements el;
    for (size_t i = 0; i < m_names.size(); ++i)
    {
        bool is_selected = (i == m_highlighted_index);
        auto&& it = m_names[i];

        auto line_num = text(ut_printer("%3d ", i+1).str()) | color(Color::YellowLight);

        if (is_selected)
        {
            el.push_back(hbox({
                line_num,
                text(it.text_old) | color(Color::RedLight),
            }) | notflex | focus);

        }
        else
        {
            el.push_back(hbox({
                line_num,
                text(it.text_old)
            }) | notflex);
        }
    }


    return el;
}

void UserInterface::setInfo(string const& s)
{
    m_message = text("  " + s) | color(Color::GreenLight);
}
void UserInterface::setWarn(string const& s)
{
    m_message = text("  " + s) | color(Color::YellowLight);
}
void UserInterface::setError(string const& s)
{
    m_message = text("  " + s) | color(Color::RedLight);
}

void UserInterface::refreshNewNames()
{
    setInfo("Press Return to rename");

    try
    {
        m_expression = fmt::Expression(m_str_expression);

        for (size_t i = 0; i < m_names.size(); ++i)
        {
            auto&& it = m_names[i];

            fmt::State fmt_state {
                .original = it.text_old,
                .index = static_cast<int>(i),
            };

            it.text_new = m_expression.toString(fmt_state);
        }
    }
    catch (exception const& e)
    {
        setError(e.what());
    }

    if (!m_names.isNewTextUnique())
    {
        setError("new names are not unique") ;
        return;
    }

    if (!m_names.hasNewNames())
    {
        setError("no names to be changed");
    }
}

void UserInterface::changeEditing()
{
    m_state = EDITING;
    setInfo("Press Enter to rename");
}

void UserInterface::changeArming()
{
    m_state = ARMING;
    setWarn("Press Enter again to confirm");
}

int UserInterface::run(filesystem::path path)
{
    if (!exists(path))
        return EXIT_FAILURE;

    auto screen = ScreenInteractive::Fullscreen();

    m_names.loadFilenames(path);

    refreshNewNames();

    // Initialize split sizes
    int split_position = 50;

    auto renderer_old_names = Renderer([&] { return vbox(createOldNameElements()) | left_vscroll_indicator | frame; });
    auto renderer_new_names = Renderer([&] { return vbox(createNewNameElements()) | frame; });

    auto input_replace = Input(&m_str_expression, "expression", {
        .multiline = false,
        .on_change = [&]{ changeEditing(); refreshNewNames(); },
    });

    // First construct the resizable split
    auto _split = ResizableSplitLeft(renderer_old_names, renderer_new_names, &split_position);

    auto _component = Container::Vertical({
        _split,
        input_replace
    });

    auto _renderer = Renderer(_component, [&]
    {
        return vbox({
            _split->Render() | flex,
            vbox({
                hbox({
                    text("> ") | ( input_replace->Focused() ? color(Color::Green) | bold : dim ),
                    input_replace->Render()
                }),
                m_message,
                hbox({
                    text(RENE_NAME " " RENE_VERSION) | dim,
                    separatorEmpty(),
                    text("path: " + path.string()) | dim
                })
            }) | border
        }) | flex;
    });

    auto _event_catch = CatchEvent(_renderer, [&](Event e)
    {
        if (e == Event::Escape)
        {
            screen.Exit();
            return true;
        }

        if (e == Event::ArrowUp)
        {
            if (m_highlighted_index > 0)
                --m_highlighted_index;
            if (isArming())
                changeEditing();
            return true;
        }

        if (e == Event::ArrowDown)
        {
            if (m_highlighted_index < m_names.size()-1)
                ++m_highlighted_index;
            if (isArming())
                changeEditing();
            return true;
        }

        if (e == Event::Return)
        {
            if (isEditing())
            {
                changeArming();
            }
            else if (isArming())
            {
                changeEditing();
                renameAllFiles(path, m_names);

                m_names.loadFilenames(path);
                refreshNewNames();
            }
            return true;
        }

        return false;
    });

    input_replace->TakeFocus();

    screen.Loop(_event_catch);
    return EXIT_SUCCESS;
}


UserInterface& UserInterface::instance()
{
    static UserInterface x;
    return x; 
}

