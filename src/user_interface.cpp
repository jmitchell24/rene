//
// Created by james on 02/05/25.
//

//
// rene
//
#include "user_interface.hpp"
#include "replacer.hpp"
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
// utilities
//


vector<string> getTestNames(size_t sz);

bool isUnique(names_type const& names)
{
    unordered_set<string> tmp;
    for (auto&& it: names)
    {
        if (tmp.contains(it.text_new))
            return false;
        tmp.insert(it.text_new);
    }
    return true;
}

bool isNew(names_type const& names)
{
    for (auto&& it: names)
    {
        if (it.text_new != it.text_old)
            return true;
    }
    return false;
}

names_type getNames(path_type path)
{
    using namespace std::filesystem;

    names_type names;

    if (!exists(path))
        return names;

    if (!is_directory(path))
        return names;

    for (auto&& it : directory_iterator(path))
    {
        if (it.is_regular_file())
            names.push_back({it.path().filename().string(), ""});
    }

    return names;
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

    rename(old_path, new_path);
}

void renameAllFiles(path_type path, names_type const& names)
{
    using namespace std::filesystem;

    if (!exists(path))
        return;

    for (auto&& it: names)
    {
        renameFile(path, it.text_old, it.text_new);
    }
}


//
// UserInterface -> Implementation
//


UserInterface::UserInterface()
{
    for (auto&& it: getTestNames(50))
    {
        m_names.push_back(Name(it));
    }
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

        if (is_selected)
        {
            ostringstream oss;
            oss << "(" << (i+1) << "/" << m_names.size() << "> ";
            el.push_back(hbox({
                text(oss.str()) | color(Color::Green) | bold,
                text(it.text_old) | inverted
            }) | notflex | focus);

        }
        else
        {
            el.push_back(hbox({
                text(" "),
                text(it.text_old)
            }) | notflex);
        }
    }


    return el;
}

void UserInterface::setInfo(string const& s)
{
    m_str_error = "";
    m_str_warn = "";
    m_str_info = s;
}
void UserInterface::setWarn(string const& s)
{
    m_str_error = "";
    m_str_warn = s;
    m_str_info = "";
}
void UserInterface::setError(string const& s)
{
    m_str_error = s;
    m_str_warn = "";
    m_str_info = "";
}

void UserInterface::refreshNewNames()
{
    setInfo("Press Return to rename");

    try
    {
        m_replacer = Replacer(m_str_replace);

        for (size_t i = 0; i < m_names.size(); ++i)
        {
            auto&& it = m_names[i];

            Replacer::Args args {
                .original = it.text_old,
                .increment = (int)i+1,
            };

            it.text_new = m_replacer.replace(args);
        }
    }
    catch (exception const& e)
    {
        setError(e.what());
    }

    if (!isUnique(m_names))
    {
        setError("new names are not unique") ;
        return;
    }

    if (!isNew(m_names))
    {
        m_str_error = "no names would be changed";
        setError("no names to be changed");
    }
}

// Component CachedRenderer(std::function<Element()> render, std::function<bool()> needs_refresh)
// {
//     class Impl : public ComponentBase
//     {
//     public:
//         explicit Impl(std::function<Element()> render, std::function<bool()> needs_refresh)
//             : render_(std::move(render)), needs_refresh_(std::move(needs_refresh))
//         {}
//
//         Element OnRender() override
//         {
//             if (needs_refresh_())
//                 cached_element_ = render_();
//             return cached_element_;
//         }
//         std::function<Element()> render_;
//         std::function<bool()> needs_refresh_;
//         Element cached_element_;
//     };
//
//     return Make<Impl>(std::move(render), std::move(needs_refresh));
// }

void UserInterface::changeEditing()
{
    m_state = EDITING;
    setInfo("Press Return to rename");
}

void UserInterface::changeArming()
{
    m_state = ARMING;
    setWarn("Press Return again to confirm");
}

int UserInterface::run(filesystem::path path)
{
    if (!exists(path))
        return EXIT_FAILURE;

    auto screen = ScreenInteractive::Fullscreen();

    m_names = getNames(path);

    refreshNewNames();

    // Initialize split sizes
    int split_position = 50;


    auto renderer_old_names = Renderer([&] { return vbox(createOldNameElements()) | left_vscroll_indicator | frame; });
    auto renderer_new_names = Renderer([&] { return vbox(createNewNameElements()) | frame; });

    auto input_replace = Input(&m_str_replace, "new name", {
        .multiline = false,
        .on_change = [&]{ changeEditing(); refreshNewNames(); },
    });

    // auto buttons_renderer = Renderer(_buttons_component, [&]
    // {
    //     if (!m_str_error.empty())
    //         return text(m_str_error) | color(Color::RedLight);
    //
    //     switch (button_state)
    //     {
    //         case EDITING:
    //             return button_rename->Render();
    //
    //         case ARMING:
    //             return hbox({
    //                 button_confirm->Render() | color(Color::GreenLight),
    //                 button_cancel->Render() | color(Color::RedLight)
    //             });
    //
    //         default:nopath_case(ButtonState);
    //     }
    //
    //     return separatorEmpty();
    // });

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
                hbox({
                    text("path: " + path.string()) | dim,
                }),
                hbox({
                    text(RENE_NAME " " RENE_VERSION) | dim,
                    separatorEmpty(),
                    text(m_str_error) | color(Color::RedLight),
                    text(m_str_warn) | color(Color::YellowLight),
                    text(m_str_info) | color(Color::GreenLight)
                })
            }) | border
        }) | flex;
    });

    auto _event_catch = CatchEvent(_renderer, [&](Event e)
    {
        if (e == Event::Escape || e == Event::q)
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
                m_names = getNames(path);
                refreshNewNames();
            }
            return true;
        }

        return false;
    });

    screen.Loop(_event_catch);
    return EXIT_SUCCESS;
}


UserInterface& UserInterface::instance()
{
    static UserInterface x;
    return x; 
}

vector<string> getTestNames(size_t sz)
{
    static vector<string> names = {
        "The Shawshank Redemption (1994)",
        "The Godfather (1972)",
        "The Dark Knight (2008)",
        "Pulp Fiction (1994)",
        "Schindler's List (1993)",
        "Forrest Gump (1994)",
        "Fight Club (1999)",
        "Inception (2010)",
        "The Matrix (1999)",
        "Goodfellas (1990)",
        "One Flew Over the Cuckoo's Nest (1975)",
        "Se7en (1995)",
        "The Lord of the Rings: The Return of the King (2003)",
        "Star Wars: Episode V - The Empire Strikes Back (1980)",
        "Titanic (1997)",
        "City of God (2002)",
        "Sunset Blvd. (1950)",
        "Casablanca (1942)",
        "Psycho (1960)",
        "12 Angry Men (1957)",
        "In the Mood for Love (2000)",
        "Life Is Beautiful (1997)",
        "The Pianist (2002)",
        "Forrest Gump (1994)",
        "The Silence of the Lambs (1991)",
        "It's a Wonderful Life (1946)",
        "La Haine (1995)",
        "The Usual Suspects (1995)",
        "Léon: The Professional (1994)",
        "North by Northwest (1959)",
        "Double Indemnity (1944)",
        "Gone with the Wind (1939)",
        "The Conversation (1974)",
        "Rashomon (1950)",
        "Alien (1979)",
        "Vertigo (1958)",
        "Taxi Driver (1976)",
        "Brazil (1985)",
        "Juno (2007)",
        "Up (2009)",
        "Parasite (2019)",
        "Moonlight (2016)",
        "The Shape of Water (2017)",
        "Green Book (2018)",
        "The Irishman (2019)",
        "Django Unchained (2012)",
        "The Social Network (2010)"
    };

    static vector<string> exts = {
        "mp4",
        "mkv",
        "avi",
        "mov",
        "wmv",
        "flv",
        "3gp",
        "rmvb",
        "iso",
        "rom"
    };


    vector<string> res;



    for (size_t i = 0; i < sz; ++i)
    {
        res.push_back(
            ut_rng.choose(names.begin(), names.end()) + "." +
            ut_rng.choose(exts.begin(), exts.end()));
    }

    return res;
}
