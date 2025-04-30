//
// Created by james on 30/04/25.
//

#include "user_interface.hpp"
using namespace rene;

//
// ftxui
//
using namespace ftxui;

//
// std
//
using namespace std;

//
//
//

// Convert string to lowercase for case-insensitive matching
std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Highlight matching characters in the string
Elements highlight_match(const std::string& str, const std::string& query) {
    Elements result;

    if (query.empty()) {
        result.push_back(text(str));
        return result;
    }

    std::string str_lower = to_lower(str);
    std::string query_lower = to_lower(query);

    size_t pos = str_lower.find(query_lower);

    if (pos == std::string::npos) {
        // No exact match, try to find individual characters
        std::vector<bool> matched(str.size(), false);
        size_t query_idx = 0;

        for (size_t i = 0; i < str.size() && query_idx < query.size(); ++i) {
            if (std::tolower(str[i]) == std::tolower(query[query_idx])) {
                matched[i] = true;
                query_idx++;
            }
        }

        if (query_idx == query.size()) {
            // All characters matched
            std::string current;
            bool is_highlighted = false;

            for (size_t i = 0; i < str.size(); ++i) {
                if (matched[i] != is_highlighted) {
                    if (!current.empty()) {
                        result.push_back(is_highlighted
                            ? text(current) | color(Color::Green) | bold
                            : text(current));
                        current.clear();
                    }
                    is_highlighted = matched[i];
                }
                current += str[i];
            }

            if (!current.empty()) {
                result.push_back(is_highlighted
                    ? text(current) | color(Color::Green) | bold
                    : text(current));
            }
        } else {
            result.push_back(text(str));
        }
    } else {
        // Exact match found
        if (pos > 0) {
            result.push_back(text(str.substr(0, pos)));
        }
        result.push_back(text(str.substr(pos, query.size())) | color(Color::Green) | bold);
        if (pos + query.size() < str.size()) {
            result.push_back(text(str.substr(pos + query.size())));
        }
    }

    return result;
}

// Check if a string matches the query (fuzzy match)
bool fuzzy_match(const std::string& str, const std::string& query) {
    if (query.empty()) {
        return true;
    }

    std::string str_lower = to_lower(str);
    std::string query_lower = to_lower(query);

    if (str_lower.find(query_lower) != std::string::npos) {
        return true;
    }

    // Check for subsequence match
    size_t str_idx = 0;
    for (char qc : query_lower) {
        bool found = false;
        while (str_idx < str_lower.size()) {
            if (str_lower[str_idx++] == qc) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    return true;
}

// Calculate match score (lower is better)
int match_score(const std::string& str, const std::string& query) {
    if (query.empty()) {
        return 0;
    }

    std::string str_lower = to_lower(str);
    std::string query_lower = to_lower(query);

    // Exact match is best
    size_t exact_pos = str_lower.find(query_lower);
    if (exact_pos != std::string::npos) {
        return exact_pos; // Prefer matches at the start
    }

    // Calculate subsequence match distance
    int score = 1000;
    size_t last_match_pos = 0;
    size_t total_distance = 0;
    bool found_all = true;

    for (char qc : query_lower) {
        bool found = false;
        for (size_t i = last_match_pos; i < str_lower.size(); ++i) {
            if (str_lower[i] == qc) {
                total_distance += (i - last_match_pos);
                last_match_pos = i + 1;
                found = true;
                break;
            }
        }
        if (!found) {
            found_all = false;
            break;
        }
    }

    if (found_all) {
        score = 500 + total_distance;
    }

    return score;
}

//
// UserInterface -> Implementation
//


UserInterface::UserInterface(itemlist_type const& items)
    : m_items_left(items), m_selected(0)
{
    m_filtered_items = m_items_left;
    m_items_right = m_items_left;

    indices_.resize(m_items_left.size());

    for (size_t i = 0; i < m_items_left.size(); ++i)
        indices_[i] = i;

    // Create components
    input_ = Input(&query_, "");

    // Create menu component
    Component menu = Container::Vertical({}, &selected_item_);
    menu_ = menu;

    auto container = Container::Vertical({
        input_,
        menu_
    });

    container_ = container;
    Refresh();
}

Component UserInterface::Build()
{
    return container_;
}

void UserInterface::Refresh()
{
    // Filter items based on query
    m_filtered_items.clear();
    indices_.clear();

    if (query_.empty())
    {
        m_filtered_items = m_items_left;
        indices_.resize(m_items_left.size());
        for (size_t i = 0; i < m_items_left.size(); ++i)
        {
            indices_[i] = i;
        }
    }
    else
    {
        vector<pair<int, size_t>> scored_items;
        for (size_t i = 0; i < m_items_left.size(); ++i)
        {
            if (fuzzy_match(m_items_left[i], query_))
            {
                int score = match_score(m_items_left[i], query_);
                scored_items.push_back({score, i});
            }
        }

        // Sort by score
        sort(scored_items.begin(), scored_items.end());

        for (const auto& pair : scored_items)
        {
            m_filtered_items.push_back(m_items_left[pair.second]);
            indices_.push_back(pair.second);
        }
    }

    // Adjust selection if needed
    if (m_filtered_items.empty())
    {
        m_selected = 0;
    }
    else if (m_selected >= m_filtered_items.size())
    {
        m_selected = m_filtered_items.size() - 1;
    }

    // Create menu elements
    Components items;
    for (size_t i = 0; i < m_filtered_items.size(); ++i)
    {
        bool is_selected = (i == m_selected);

        auto on_enter = [this, i]
        {
            m_selected = i;
            selected_item_ = indices_[i];
            return true;
        };

        items.push_back(Button("", on_enter, ButtonOption::Ascii()));
    }

    menu_->DetachAllChildren();
    for (auto&& it: items)
        menu_->Add(it);
    //menu_->Add(Container::Vertical(items));
}

int UserInterface::GetSelectedIndex() const
{
    return selected_item_;
}



Element UserInterface::Render()
{


    // Show prompt and input
    Elements prompt_elements;
    prompt_elements.push_back(text("> ") | color(Color::Green) | bold);
    prompt_elements.push_back(text(query_));
    prompt_elements.push_back(text("_") | blink);

    Element prompt = hbox(prompt_elements);

    // Show number of matches
    Element matches_counter = text("") | color(Color::GrayDark);
    if (!m_items_left.empty())
    {
        string counter = to_string(m_filtered_items.size()) + "/" +
                              to_string(m_items_left.size());
        matches_counter = text(counter) | color(Color::GrayDark);
    }

    // Format menu entries
    Elements menu_entries;
    for (size_t i = 0; i < m_filtered_items.size(); ++i)
    {
        bool is_selected = (i == m_selected);

        Elements line;
        Elements highlighted = highlight_match(m_filtered_items[i], query_);

        if (is_selected)
        {
            line.push_back(text("> ") | color(Color::Green) | bold);
            Element line_content = hbox(highlighted);
            line.push_back(line_content | inverted);
            menu_entries.push_back(hbox(line) | focus);
        }
        else
        {
            line.push_back(text("  "));
            Element line_content = hbox(highlighted);
            line.push_back(line_content);
            menu_entries.push_back(hbox(line));
        }


    }

    // Calculate max height (limit to 10 items)
    size_t max_height = std::min(m_filtered_items.size(), size_t(10));

    Element menu;
    if (menu_entries.empty())
        menu = text("No matches") | color(Color::GrayDark);
    else
        menu = vbox(menu_entries) ;



    // Header with stats
    Element header = hbox({
        prompt,
        filler(),
        matches_counter
    });

    return vbox({
        header ,
        separator(),
        menu | vscroll_indicator | yframe
    });
}

bool UserInterface::OnEvent(Event event)
{
    // Process input
    if (input_->OnEvent(event))
    {
        Refresh();
        return true;
    }

    // Handle navigation
    if (event.is_mouse() || m_filtered_items.empty())
    {
        return false;
    }

    if (event == Event::ArrowDown || event == Event::Tab)
    {
        m_selected = (m_selected + 1) % m_filtered_items.size();
        selected_item_ = indices_[m_selected];
        return true;
    }

    if (event == Event::ArrowUp || event == Event::TabReverse)
    {
        m_selected = (m_selected + m_filtered_items.size() - 1) % m_filtered_items.size();
        selected_item_ = indices_[m_selected];
        return true;
    }

    if (event == Event::Return && !m_filtered_items.empty())
    {
        selected_item_ = indices_[m_selected];
        return true;
    }

    return false;
}

