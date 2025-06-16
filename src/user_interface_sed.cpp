//
// Created by james on 13/06/25.
//





//
// rene
//
#include "user_interface_sed.hpp"
#include "fmt.hpp"
#include "rene.hpp"
#include "ut/time.hpp"
#include "virtual_line_list.hpp"
using namespace rene;

//
// ftxui
//
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/loop.hpp>
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
// UserInterface -> Implementation
//


UserInterfaceSed::UserInterfaceSed()
{
    setInfo("test");
}

void UserInterfaceSed::updateHighlightedIndex(size_t new_idx)
{
    if (m_highlighted_index == new_idx)
        return;

    if (new_idx >= m_names.size())
        return;

    m_highlighted_index = new_idx;
}

void UserInterfaceSed::setInfo(string const& s)
{
    m_message = text("  " + s) | color(Color::GreenLight);
}
void UserInterfaceSed::setWarn(string const& s)
{
    m_message = text("  " + s) | color(Color::YellowLight);
}
void UserInterfaceSed::setError(string const& s)
{
    m_message = text("  " + s) | color(Color::RedLight);
}

void UserInterfaceSed::refreshOldNames()
{

}

void UserInterfaceSed::refreshNewNames()
{
    setInfo("Press Return to rename");

    try
    {
        m_expression = fmt::Expression(m_str_expression);

        for (size_t i = 0; i < m_names.size(); ++i)
        {
            auto&& it = m_names[i];

            fmt::State fmt_state {
                .original = it.textOld(),
                .matches = { "asdf" },
                .index = static_cast<int>(i),
            };

            it.setTextNew(m_expression.toString(fmt_state));
        }


        //m_new_names = createNewNameElements();
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

void UserInterfaceSed::changeEditing()
{
    m_state = EDITING;
    setInfo("Press Enter to rename");
}

void UserInterfaceSed::changeArming()
{
    m_state = ARMING;
    setWarn("Press Enter again to confirm");
}

/*
TODO: Resizing

- set initial position to just enough for old text, with a margin
- if left split is too small to fit old text, display an error icon in split, message below
- if right split is too small to fit new text, display an error icon in split, message below
- read text() code, see if it's doing anything when it doesn't have enough space (feels like it does, and i want to change that behavior, elipsis or otherwise)

- read frame() code, start considering fully custom renderer / component for names list
*/

VirtualLine getVirtualLineOld(Name const& n, regex const& r)
{
    static vector<Color::Palette16> colors = {
        Color::RedLight,
        Color::GreenLight,
        Color::YellowLight,
        Color::BlueLight,
        Color::MagentaLight,
        Color::CyanLight
    };

    VirtualLine vl;

    size_t col=0;

    for (auto&& it: n.getSubsOld(r))
    {
        for (auto&& jt: strview(n.textOld(), {it.begin, it.end}))
        {
            Pixel p;
            p.character = jt;

            p.inverted = it.highlight;
            if (it.highlight)
                p.foreground_color = Color(colors[col]);
            vl.pixels.push_back(p);
        }

        col = (col + 1) % colors.size();
    }

    return vl;
}

int UserInterfaceSed::run(NameList const& name_list)
{


    auto screen = ScreenInteractive::Fullscreen();
    auto render_time = 0_seconds;

    m_names = name_list;

    refreshNewNames();
    refreshOldNames();

    int split_position = m_names.maxExtentOld() + 6;

    int vlist_old_offset = 0;
    int vlist_new_offset = 0;

    auto vlist_old_names = VirtualList({

        .show_scrollbar = true,
        .show_line_numbers = true,
        .offset = &vlist_old_offset,
        .view_count = (int)m_names.size(),
        .view_func = [&](int i) { return getVirtualLineOld(m_names[i], m_regex); }

    });

    auto vlist_new_names = VirtualList({

        .show_scrollbar = false,
        .show_line_numbers = false,
        .offset = &vlist_new_offset,
        .view_count = (int)m_names.size(),
        .view_func = [&](int i) { return VirtualLine(m_names[i].textNew()); }

    });

    auto input_match = Input(&m_str_match, "match", {

        .multiline = false,
        .on_change = [&]
        {
            try { m_regex = regex(m_str_match); } catch (...) {}
        }

    });

    auto input_replace = Input(&m_str_expression, "expression", {

        .multiline = false,
        .on_change = [&]{ changeEditing(); refreshNewNames(); },

    });



    // First construct the resizable split
    auto _split = ResizableSplitLeft(vlist_old_names, vlist_new_names, &split_position);

    auto _component = Container::Vertical({
        _split,
        input_match,
        input_replace
    });

    auto _renderer = Renderer(_component, [&]
    {
        return vbox({
            _split->Render() | flex,
            vbox({
                hbox({
                    text("> ") | ( input_match->Focused() ? color(Color::Green) | bold : dim ),
                    input_match->Render()
                }),

                hbox({
                    text("> ") | ( input_replace->Focused() ? color(Color::Green) | bold : dim ),
                    input_replace->Render()
                }),

                m_message,

                hbox({
                    text(RENE_NAME " " RENE_VERSION) | dim,
                    separatorEmpty(),
                    text("path: " ) | dim
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
            vlist_new_offset--;
            vlist_old_offset--;

            if (m_highlighted_index > 0)
            {
                updateHighlightedIndex(m_highlighted_index-1);
            }

            if (isArming())
                changeEditing();
            return true;
        }

        if (e == Event::ArrowDown)
        {
            vlist_new_offset++;
            vlist_old_offset++;

            if (m_highlighted_index < m_names.size()-1)
            {
                updateHighlightedIndex(m_highlighted_index+1);
            }
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
                //renameAllFiles(path, m_names);
                //m_names.loadFilenames(path);
                refreshNewNames();
            }
            return true;
        }

        return false;
    });

    input_match->TakeFocus();

    Loop loop(&screen, _event_catch);
    while (!loop.HasQuitted())
    {
        auto scoped_timer = timer::scope(render_time);
        loop.RunOnce();
    }

    return EXIT_SUCCESS;
}


UserInterfaceSed& UserInterfaceSed::instance()
{
    static UserInterfaceSed x;
    return x;
}