//
// Created by james on 21/05/25.
//

#pragma once


//
// ftxui
//

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>

//
// std
//

#include <string>
#include <vector>
#include <format>
#include <algorithm>
#include <cassert>

namespace ftxui
{
    /// A list of pixels, intended to be rendered on a line
    /// For use with VirtualLineList.
    struct VirtualLine
    {
        std::vector<Pixel> pixels;

        inline VirtualLine()
        {}

        inline VirtualLine(std::string const& s, Color c = Color::Default)
        { set(s, c); }

        inline bool empty() const
        { return pixels.empty(); }

        inline size_t size() const
        { return pixels.size(); }

        inline Pixel const& at(size_t i) const
        { assert(i < pixels.size()); return pixels[i]; }

        void set(std::string const& s, Color c = Color::Default);
        void append(std::string const& s, Color c = Color::Default);
        int render(int col_max, int col_start, int row, Screen& screen);
        static int getVirtualLineSize(std::string const& s);
    };

    struct VirtualLineListOptions
    {
        bool show_scrollbar = true;

        bool show_line_numbers = true;

        Ref<int> offset = int(0);

        int view_count = int(100);

        std::function<VirtualLine(int)> view_func = [](int i) { return VirtualLine(std::format("line {}", i)); };
    };

    class VirtualListNode : public Node
    {
    public:
        explicit VirtualListNode(VirtualLineListOptions options);

        // Step 1: Compute layout requirement. Tell parent what dimensions this
        //         element wants to be.
        //         Propagated from Children to Parents.
        void ComputeRequirement() override;

        // Step 2: Assign this element its final dimensions.
        //         Propagated from Parents to Children.
        void SetBox(Box box) override;

        // Step 3: (optional) Selection
        //         Propagated from Parents to Children.
        void Select(Selection& selection) override;

        // Step 4: Draw this element.
        void Render(Screen& screen) override;

    private:
        VirtualLineListOptions m_options;

        Box m_box = {};
        int m_cols = 0;
        int m_rows = 0;
        int m_lineno_width = 0;

        int renderScrollBar(int col, Screen& screen);
        int renderLineNumbers(int col, Screen& screen);
        int renderLine(int col, Screen& screen);

        void sanitizeOptions();
        int visibleLineCount() const;
        VirtualLine getLineNumberLine(int line_no) const;
    };

    inline static Component VirtualList(VirtualLineListOptions options)
    {
        class Impl : public ComponentBase
        {
        public:
            Element m_element;
            explicit Impl(VirtualLineListOptions options)
                : m_element(Make<VirtualListNode>(std::move(options)))
            { }

            Element OnRender() override { return m_element; }
        };

        return Make<Impl>(std::move(options));
    }
}