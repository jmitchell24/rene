//
// Created by james on 21/05/25.
//

#include "virtual_line_list.hpp"
using namespace ftxui;

//
// ut
//
#include <ut/string.hpp>
using namespace ut;

//
// std
//
#include <cmath>
using namespace std;

#define SCROLL_CHAR_MIDDLE  ("┃")
#define SCROLL_CHAR_TOP     ("╹")
#define SCROLL_CHAR_BOTTOM  ("╻")
#define SCROLL_CHAR_EMPTY   (" ")

#define LINE_NUMBER_FORMAT ("%*.d ")

//
// helper functions
//

inline static int getDigitCount(int x)
{
    if (x == 0)
        return 1;
    int abs_x = abs(x);
    return static_cast<int>(floor(std::log10(abs_x))) + 1;
}

inline static int convertRange(int old_min, int old_max, int old_val, int new_min, int new_max)
{
    // Calculate the normalized value (0-1)
    float normalized = static_cast<float>(old_val - old_min) / static_cast<float>(old_max - old_min);
    // Scale and shift to fit in the new range
    return new_min + (int)ceil(normalized * (new_max - new_min));
}

//
// Implementation -> VirtualList
//

void VirtualLine::set(std::string const& s, Color c)
{
    pixels.clear();
    for (auto&& it: s)
    {
        Pixel p;
        p.character = it;
        p.foreground_color = c;
        pixels.push_back(p);
    }
}

void VirtualLine::append(std::string const& s, Color c)
{
    for (auto&& it: s)
    {
        Pixel p;
        p.character = it;
        p.foreground_color = c;
        pixels.push_back(p);
    }
}

int VirtualLine::render(int col_max, int col_start, int row, Screen& screen) const
{
    for (int col = col_start; col <= col_max; ++col)
    {
        int pi = col - col_start;
        if (pi >= pixels.size())
            return col;
        screen.PixelAt(col, row) = pixels[pi];
    }
    return col_max;
}

int VirtualLine::getVirtualLineSize(std::string const& s)
{
    return s.size();
}

//
// Implementation -> VirtualListNode
//


VirtualListNode::VirtualListNode(VirtualLineListOptions options)
    : m_options(options), m_lineno_width(getLineNumberLine(m_options.view_count).size())
{ }

void VirtualListNode::ComputeRequirement()
{
    requirement_ = {};
}

void VirtualListNode::SetBox(Box box)
{
    m_box = box;
    m_cols = 1 + box.x_max - box.x_min;
    m_rows = 1 + box.y_max - box.y_min;
}

void VirtualListNode::Select(Selection& selection)
{
    Node::Select(selection);
}

void VirtualListNode::Render(Screen& screen)
{
    sanitizeOptions();

    int col = m_box.x_min; // current column

    if (m_options.show_scrollbar)
        col = renderScrollBar(col, screen);
    else
        col += 1;

    if (m_options.show_line_numbers)
        col = renderLineNumbers(col, screen);

    col = renderLine(col, screen);
}

int VirtualListNode::renderScrollBar(int col, Screen& screen)
{
    if (m_cols-col < 1)
        return col;

    // Calculate scrollbar position and size
    int scrollbar_start = convertRange(0, m_options.view_count, m_options.offset(), 0, m_rows);
    int scrollbar_size = 0;


    if (m_options.view_count <= m_rows)
    {
        scrollbar_start = 0;
        scrollbar_size = m_rows;
    }
    else
    {
        // visible line count
        float vl = (float)min(m_rows, (int)m_options.view_count);
        float vl_ratio = vl / m_options.view_count;

        // Calculate scrollbar size (minimum 1 character)
        scrollbar_size = max(1, static_cast<int>(vl_ratio * m_rows));
    }

    for (int i = 0; i < m_rows; ++i)
    {
        int row = m_box.y_min + i;

        string scrollbar_char = SCROLL_CHAR_EMPTY; // empty

        if (scrollbar_size == 1)
        {
            if (i == scrollbar_start)
                scrollbar_char = SCROLL_CHAR_MIDDLE;
        }
        else if (scrollbar_size == 2)
        {
            if (i == scrollbar_start)
                scrollbar_char = SCROLL_CHAR_MIDDLE;
            else if (i == scrollbar_start + 1)
                scrollbar_char = SCROLL_CHAR_MIDDLE;
        }
        else
        {
            if (i == scrollbar_start)
                scrollbar_char = SCROLL_CHAR_BOTTOM;
            else if (i == scrollbar_start + scrollbar_size - 1)
                scrollbar_char = SCROLL_CHAR_TOP;
            else if (i > scrollbar_start && i < scrollbar_start + scrollbar_size - 1)
                scrollbar_char = SCROLL_CHAR_MIDDLE;
        }

        // Draw the scrollbar character
        auto&& cell = screen.PixelAt(col, row);
        cell.character = scrollbar_char;
        cell.foreground_color = Color::Red;
    }

    return col+1;
}

int VirtualListNode::renderLineNumbers(int col, Screen& screen)
{
    int vsz = visibleLineCount();

    for (int vi = 0; vi < vsz; ++vi)
    {
        int row = m_box.y_min + vi;
        int virt_line = vi + m_options.offset() + 1;

        auto line = getLineNumberLine(virt_line);
        line.render(m_box.x_max, col, row, screen);
    }

    return col + m_lineno_width;
}

int VirtualListNode::renderLine(int col, Screen& screen)
{
    int vsz = visibleLineCount();

    for (int vi = 0; vi < vsz; ++vi)
    {
        int row = m_box.y_min + vi;
        int virt_line = m_options.offset() + vi;

        // Get line content from the line view function
        auto line = m_options.view_func ? m_options.view_func(virt_line) : VirtualLine();

        line.render(m_box.x_max, col, row, screen);
    }

    return m_box.x_max+1;
}

void VirtualListNode::sanitizeOptions()
{
    if (m_options.view_count < 0)
        m_options.view_count = 0;

    int off_min = 0;
    int off_max = max(0, m_options.view_count - m_rows);

    auto&& off = m_options.offset();

    if (off < off_min) off = off_min;
    if (off > off_max) off = off_max;
}

int VirtualListNode::visibleLineCount() const
{
    return min(m_rows, m_options.view_count);
}

VirtualLine VirtualListNode::getLineNumberLine(int lineno) const
{
    int width = getDigitCount(m_options.view_count);
    auto s = ut_printer(LINE_NUMBER_FORMAT, width, lineno).str();
    return VirtualLine(s, Color::YellowLight);
}

