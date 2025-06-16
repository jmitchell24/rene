//
// Created by james on 09/05/25.
//

#pragma once
#include "ftxui/dom/elements.hpp"


namespace ftxui
{


    /// @brief Display a vertical scrollbar to the left.
    /// colors.
    /// @ingroup dom
    inline static Element left_vscroll_indicator(Element child)
    {
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

}
