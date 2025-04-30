//
// Created by james on 30/04/25.
//

#pragma once



//
// ftxui
//
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component_options.hpp>

//
// std
//
#include <vector>
#include <string>

namespace rene
{
    using itemlist_type = std::vector<std::string>;

    class UserInterface
    {
    public:
        UserInterface(itemlist_type const& items);

        ftxui::Component Build();

        void Refresh();

        int GetSelectedIndex() const;



        ftxui::Element Render();

        bool OnEvent(ftxui::Event event);

    private:
        itemlist_type m_items_left;
        itemlist_type m_items_right;

        itemlist_type m_filtered_items;
        std::vector<size_t> indices_;
        std::string query_;
        size_t m_selected;
        int selected_item_ = -1;

        ftxui::Component input_;
        ftxui::Component menu_;
        ftxui::Component container_;
    };
}