//
// Created by james on 13/06/25.
//





//
// rene
//
#include "user_interface_sed.hpp"

#include "find_replace.hpp"
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

VirtualLine getVirtualLine(strparam s, emlist_type const& emlist, Pixel p_em = {}, Pixel p_none = {})
{
    VirtualLine vl;
    for (auto& it: emlist)
    {
        for (auto&& jt: s.withIndices(it.begin, it.end))
        {
            switch (it.kind)
            {
                case Em::NONE:
                    p_none.character = jt;
                    vl.pixels.push_back(p_none);
                    break;

                case Em::MATCH:
                    p_em.character = jt;
                    p_em.foreground_color = Color(Color::LightCoral);
                    vl.pixels.push_back(p_em);
                    break;

                case Em::VAR:
                    p_em.character = jt;
                    p_em.foreground_color = Color(Color::LightGreen);
                    vl.pixels.push_back(p_em);
                    break;

                default:nopath_case(Em::Kind);
            }
        }
    }
    return vl;
}

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

void UserInterfaceSed::refreshNewNames(fmt::Expression& expr)
{
    setInfo("Press Return to rename");

    try
    {
        expr = fmt::Expression(m_str_expression);

        for (size_t i = 0; i < m_names.size(); ++i)
        {
            auto&& it = m_names[i];

            fmt::Expression::State fmt_state {
                .original = it.textOld(),
                .index = static_cast<int>(i),
                .total = static_cast<int>(m_names.size()),
            };

            it.setTextNew(expr.getResult(fmt_state).text);
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


int UserInterfaceSed::run(NameList const& name_list)
{

    auto screen = ScreenInteractive::Fullscreen();
    auto render_time = 0_seconds;

    FindReplace find_replace;

    m_names = name_list;

    refreshNewNames(find_replace.m_replace);
    refreshOldNames();

    int split_position = m_names.maxExtentOld() + 6;

    int vlist_old_offset = 0;
    int vlist_new_offset = 0;

    auto vlist_old_names = VirtualList({

        .show_scrollbar = true,
        .show_line_numbers = true,
        .show_flags = false,
        .offset = &vlist_old_offset,
        .view_count = (int)m_names.size(),
        .view_func = [&](int i)
        {
            auto s = strview(m_names[i].textOld());
            auto r = find_replace.getFindResult(s);
            auto p = Pixel();
            p.inverted = true;

            return getVirtualLine(s, r.emlist, p);
        }

    });

    auto vlist_new_names = VirtualList({

        .show_scrollbar = false,
        .show_line_numbers = false,
        .show_flags = true,
        .offset = &vlist_new_offset,
        .view_count = (int)m_names.size(),
        .view_func = [&](int i)
        {
            FindReplace::State state {
                .index = static_cast<int>(i),
                .total = static_cast<int>(m_names.size())
            };

            auto res = find_replace.getReplaceResult(m_names[i].textOld(), state);
            auto p = Pixel();

            auto vl = getVirtualLine(res.replace_text, res.replace_emlist, p);
            vl.flag.character = "•";
            vl.flag.dim = true;
            return vl;
        }

    });

    auto input_match = Input(&m_str_match, "find", {

        .multiline = false,
        .on_change = [&]
        {
            //try { m_regex = regex(m_str_match); } catch (...) {}
            find_replace.setFind(m_str_match);
        }

    });

    auto input_replace = Input(&m_str_expression, "replace", {

        .multiline = false,
        .on_change = [&]
        {
            find_replace.setReplace(m_str_expression);
            changeEditing(); refreshNewNames(find_replace.m_replace);
        },

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
                    text("• ") | ( input_match->Focused() ? color(Color::Green) | bold : dim ),
                    input_match->Render()
                }),

                hbox({
                    text("• ") | ( input_replace->Focused() ? color(Color::Green) | bold : dim ),
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
                refreshNewNames(find_replace.m_replace);
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