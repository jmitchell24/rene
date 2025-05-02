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

    class UserInterface final : public ftxui::ComponentBase
    {
    public:
        explicit UserInterface(ftxui::Closure exit_closure);
        ~UserInterface() override;
        void Refresh();

        int GetSelectedIndex() const;

        ftxui::Component createComponent();

        //ftxui::Element Render() const;


        ftxui::Element OnRender() override;
        bool OnEvent(ftxui::Event event) override;

    private:
        itemlist_type m_items_left;
        itemlist_type m_items_right;

        itemlist_type m_filtered_items;
        std::vector<size_t> m_indices;
        std::string m_query;
        size_t m_selected;
        int m_selected_item = -1;

        ftxui::Component m_input;
        ftxui::Component m_split;
        ftxui::Closure m_exit_closure;
        //ftxui::Component m_menu;
        //ftxui::Component m_container;
    };
}

