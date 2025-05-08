//
// Created by james on 02/05/25.
//

//
// rene
//
#include "user_interface.hpp"
#include "replacer.hpp"
#include "utility.hpp"
#include "rene.hpp"
using namespace rene;

//
// ftxui
//
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component_options.hpp>
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
using namespace std;

//
// utilities
//

/// @brief Display a vertical scrollbar to the left.
/// colors.
/// @ingroup dom
Element ftxui::left_vscroll_indicator(Element child) {
    class Impl : public Node {
    public:
        explicit Impl(Element child) : Node(unpack(std::move(child))) {}

        void ComputeRequirement() override
        {
            Node::ComputeRequirement();
            requirement_ = children_[0]->requirement();
            requirement_.min_x++;
        }

        void SetBox(Box box) override
        {
            box_ = box;
            box.x_min++;
            children_[0]->SetBox(box);
        }

        void Render(Screen& screen) final
        {
            Node::Render(screen);

            Box const& stencil = screen.stencil;

            int size_inner = box_.y_max - box_.y_min;
            if (size_inner <= 0)
                return;

            int size_outer = stencil.y_max - stencil.y_min + 1;
            if (size_outer >= size_inner)
                return;

            int size = 2 * size_outer * size_outer / size_inner;
            size = std::max(size, 1);

            int start_y =
                2 * stencil.y_min +  //
                2 * (stencil.y_min - box_.y_min) * size_outer / size_inner;

            int x = stencil.x_min;
            for (int y = stencil.y_min; y <= stencil.y_max; ++y)
            {
                int  y_up   = 2 * y + 0;
                int  y_down = 2 * y + 1;
                bool up     = (start_y <= y_up) && (y_up <= start_y + size);
                bool down   = (start_y <= y_down) && (y_down <= start_y + size);

                char const* c = up ? (down ? "┃" : "╹") : (down ? "╻" : " ");  // NOLINT
                screen.PixelAt(x, y).character = c;
                screen.PixelAt(x, y).foreground_color = Color::Red;
            }
        }
    };
    return std::make_shared<Impl>(std::move(child));
}

vector<string> getTestNames(size_t sz);

// static Elements createItemElements(Replace const& replace, itemlist_type const& items, int selected_index, bool prefix)
// {
//     Elements el;
//     for (size_t i = 0; i < items.size(); ++i)
//     {
//         bool is_selected = (i == selected_index);
//         auto&& it = items[i];
//
//         if (is_selected)
//         {
//             if (prefix)
//             {
//                 ostringstream oss;
//                 oss << "(" << (i+1) << "/" << items.size() << "> ";
//                 el.push_back(hbox({
//                     text(oss.str()) | color(Color::Green) | bold,
//                     text(it) | inverted
//                 }) | notflex | focus);
//             }
//             else
//             {
//                 el.push_back(hbox({
//                     text(" "),
//                     text(it) | inverted
//                 }) | notflex | focus);
//             }
//
//         }
//         else
//         {
//             if (smatch m; regex_search(it, m, replace.match))
//             {
//                 el.push_back(hbox({
//                     text(" "),
//                     text(m.prefix()),
//                     text(m.str()) | bgcolor(Color::Blue) | inverted,
//                     text(m.suffix())
//                 }) | notflex);
//             }
//             else
//             {
//                 el.push_back(hbox({
//                     text(" "),
//                     text(it)
//                 }) | notflex);
//             }
//
//
//         }
//     }
//
//
//     return el;
// }


//
// Name -> Implementation
//

// void Name::updateMatches(regex const& r)
// {
//     auto beg = sregex_iterator(text_old.begin(), text_old.end(), r);
//     auto end = sregex_iterator();
//
//     matches.clear();
//     for (auto i = beg; i != end; ++i)
//         matches.push_back(*i);
// }



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

#if 0
void callStringSegments(string const& text,
    vector<smatch> const& matches,
    function<void(string seg, bool is_match)> const& callback)
{
    // If there are no matches, just call the callback with the entire text as a non-match
    if (matches.empty()) {
        callback(text, false);
        return;
    }

    size_t lastPos = 0;

    // Process each match and the text between matches
    for (const auto& match : matches) {
        size_t matchPos = match.position();

        // If there's text before this match, call the callback with it as a non-match
        if (matchPos > lastPos) {
            string nonMatchText = text.substr(lastPos, matchPos - lastPos);
            callback(nonMatchText, false);
        }

        // Call the callback with the matched text
        callback(match.str(), true);

        // Update lastPos to be after this match
        lastPos = matchPos + match.length();
    }

    // If there's any remaining text after the last match, call the callback with it
    if (lastPos < text.length()) {
        string remainingText = text.substr(lastPos);
        callback(remainingText, false);
    }
}
#endif

Elements UserInterface::createReplaceElements()
{
    Elements el;

    for (size_t i = 0; i < m_names.size(); ++i)
    {
        bool is_selected = (i == m_highlighted_index);
        auto&& it = m_names[i];

        if (is_selected)
            el.push_back(text(" " + it.text_new) | inverted | focus);
        else
            el.push_back(text(" " + it.text_new));
    }
    return el;
}


Elements UserInterface::createMatchElements()
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
            // if (smatch m; regex_search(it, m, m_regex))
            // {
            //
            //     el.push_back(hbox({
            //         text(" "),
            //         text(m.prefix()),
            //         text(m.str()) | bgcolor(Color::Blue) | inverted,
            //         text(m.suffix())
            //     }) | notflex);
            // }
            // else
            // {

            // }

            Elements hel = { text(" ") };

            // callStringSegments(it.text_old, it.matches, [&](string const& s, bool is_match)
            // {
            //     if (is_match)
            //         hel.push_back(text(s) | bgcolor(Color::Blue) | inverted);
            //     else
            //         hel.push_back(text(s));
            // });

            el.push_back(hbox({
            text(" "),
            text(it.text_old)
            }) | notflex);
        }
    }


    return el;
}

void UserInterface::updateReplaceList()
{
    try
    {
        m_str_error = "";
        m_replacer = Replacer(m_str_user_replace);

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
        m_str_error = e.what();
    }

}

// void UserInterface::updateMatchList()
// {
//     try
//     {
//         m_regex = m_str_user_match;
//
//         for (auto&& it: m_names)
//         {
//             it.updateMatches(m_regex);
//             it.updateNewText(m_replacer);
//         }
//
//     }
//     catch (exception const& e)
//     {
//         m_str_error = e.what();
//     }
// }

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

int UserInterface::run()
{
    auto screen = ScreenInteractive::Fullscreen();

    //updateMatchList();
    updateReplaceList();

    // Initialize split sizes
    int split_position = 50;
    int split_position_prev = 0;

    // Create the list components
    auto match_names_renderer = Renderer(
        [&] { return vbox(createMatchElements()) | left_vscroll_indicator | frame; });

    auto replace_names_renderer = Renderer(
        [&] { return vbox(createReplaceElements()) | frame; });

    auto input_field_replace = Input(&m_str_user_replace, "replace", {
        .multiline = false,
        .on_change = [&]{ updateReplaceList(); }
    });

    auto button = Button("mode", {}, ButtonOption::Ascii());



    // First construct the resizable split
    auto _split = ResizableSplitLeft(match_names_renderer, replace_names_renderer, &split_position);

    auto _component = Container::Vertical({
        _split,
        input_field_replace
    });

    auto _renderer = Renderer(_component, [&]
    {
        return vbox({
            _split->Render() | flex,
            vbox({
                hbox({
                    text("> ") | ( input_field_replace->Focused() ? color(Color::Green) | bold : dim ),
                    input_field_replace->Render()
                }),
                hbox({
                    text(RENE_NAME " " RENE_VERSION) | dim,
                    separatorEmpty(),
                    text(m_str_error) | color(Color::OrangeRed1),
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
            split_position_prev = 0;
            return true;
        }

        if (e == Event::ArrowDown)
        {
            if (m_highlighted_index < m_names.size()-1)
            {
                ++m_highlighted_index;
            }
            split_position_prev = 0;
            return true;
        }

        split_position_prev = split_position;
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
