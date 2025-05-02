//
// Created by james on 02/05/25.
//

//
// rene
//
#include "user_interface.hpp"
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
// using namespace ut;

//
// std
//
#include <cstdlib>
using namespace std;
static vector<string> getTestItems()
{
    // Return some demo items if file reading fails
    return {

        "user.rb",
        "users_controller.rb",
        "index.html.erb",
        "show.html.erb",
        "new.html.erb",
        "edit.html.erb",
        "post.rb",
        "posts_controller.rb",
        "comment.rb",
        "comments_controller.rb",
        "category.rb",
        "categories_controller.rb",
        "tag.rb",
        "tags_controller.rb",
        "profile.rb",
        "profiles_controller.rb",
        "attachment.rb",
        "attachments_controller.rb",
        "notification.rb",
        "notifications_controller.rb",
        "message.rb",
        "messages_controller.rb",
        "subscription.rb",
        "subscriptions_controller.rb",
        "application_helper.rb",
        "application.html.erb",
        "dashboard.html.erb",
        "admin.rb",
        "admins_controller.rb",
        "event.rb",
        "events_controller.rb",
        "product.rb",
        "products_controller.rb",
        "order.rb",
        "orders_controller.rb",
        "invoice.rb",
        "invoices_controller.rb",
        "session_controller.rb",
        "authentication.rb",
        "mailer.rb",
        "seed.rb",
        "routes.rb",
        "schema.rb",
        "database.yml",
        "Gemfile",
        "Rakefile",
        "application.rb",
        "environment.rb",
        "boot.rb",
        "development.rb"

  };
}

string g_query = "";
vector<string> g_list_left = getTestItems();
vector<string> g_list_right = g_list_left;
int g_selected_index = 0;

#include <sstream>

static Elements createItemElements(vector<string> const& items, int selected_index, bool prefix)
{
    Elements el;
    for (size_t i = 0; i < items.size(); ++i)
    {
        bool is_selected = (i == selected_index);
        auto&& it = items[i];

        if (is_selected)
        {
            if (prefix)
            {
                ostringstream oss;
                oss << "(" << (i+1) << "/" << items.size() << "> ";
                el.push_back(hbox({
                    text(oss.str()) | color(Color::Green) | bold,
                    text(it) | inverted
                }) | notflex | focus);
            }
            else
            {
                el.push_back(hbox({
                    text(" "),
                    text(it) | inverted
                }) | notflex | focus);
            }

        }
        else
        {
            el.push_back(hbox({
                text(" "),
                text(it)
            }) | notflex);
        }
    }


    return el;
}


/// @brief Display a vertical scrollbar to the right.
/// colors.
/// @ingroup dom
Element my_vscroll_indicator(Element child) {
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

#define TYPE_RE std::function<Element()>
#define TYPE_EV std::function<bool(Event)>
Component RenderEvent(TYPE_RE render, TYPE_EV event) {
    class Impl : public ComponentBase {
    public:
        explicit Impl(TYPE_RE render, TYPE_EV event)
            : render_(move(render)), event_(move(event)) {}
        Element OnRender() override { return render_(); }
        bool OnEvent(Event e) override { return event_(e); }
        TYPE_RE render_; TYPE_EV event_;
    };

    return Make<Impl>(move(render), move(event));
}
#undef TYPE_RE
#undef TYPE_EV

//
// UserInterface -> Implementation
//

UserInterface::UserInterface()
{}

string const& UserInterface::expression() const
{
    return g_query;
}

void UserInterface::updateRightList(renamer_type renamer)
{
    g_list_right.clear();
    for (size_t i = 0; i < g_list_left.size(); ++i)
    {
        RenameArgs ra {
            .index = i,
            .filename = g_list_left[i],
        };

        g_list_right.push_back(renamer(ra));
    }

}

int UserInterface::run(renamer_type renamer)
{
    auto screen = ScreenInteractive::Fullscreen();


    updateRightList(renamer);

    // Create the list components
    auto right = Renderer([] {
        return vbox(createItemElements(g_list_right, g_selected_index, false)) | frame | notflex;
    });

    auto left = Renderer([] {
        return vbox(createItemElements(g_list_left, g_selected_index, true)) | my_vscroll_indicator | frame | notflex;
    });

    // Create the input component with fixed height
    auto input_field = Input(&g_query, "expression", {
        .transform = [](InputState state) {

            state.element |= color(Color::White);

            if (state.is_placeholder)   state.element |= dim;
            if (state.hovered)          state.element |= bgcolor(Color::GrayDark);

            return state.element;
          },
        .multiline = false,
        .on_change = [&]{ updateRightList(renamer); }

    });

    auto input_component = Renderer(input_field, [&] {
        return vbox({
            hbox({
                text("> ") | color(Color::Green) | bold,
                input_field->Render(),
            }),
            hbox({
                text("rené 25.5.2") | dim,
                separatorEmpty(),
                text("/path/to/files") | color(Color::OrangeRed1),
                separatorEmpty(),
                text("opt1=yes") | color(Color::GreenYellow),
                separatorEmpty(),
                text("opt2=123") | color(Color::GreenYellow),
                separatorEmpty(),
                text("opt3='...'") | color(Color::GreenYellow),
                separatorEmpty(),
            })
        }) | border;
    });

    // Initialize split sizes
    int left_size = 50;

    // First construct the resizable split
    auto split = ResizableSplitLeft(left, right, &left_size);

    // Create a component structure where the lists can be scrolled/resized
    // but the input field is fixed at the bottom with a guaranteed minimum height

    auto fn_render = [&] {
        return vbox({
            split->Render() | flex,
            input_component->Render()
        }) | flex;
    };

    auto fn_event = [&](Event e)
    {
        if (e == Event::Escape || e == Event::q)
        {
            screen.Exit();
            return true;
        }

        if (e == Event::ArrowUp)
        {
            if (g_selected_index > 0)
                --g_selected_index;
            return true;
        }

        if (e == Event::ArrowDown)
        {
            if (g_selected_index < g_list_left.size()-1)
            {
                ++g_selected_index;
            }
            return true;
        }


        if (input_field->OnEvent(e))
            return true;
        return split->OnEvent(e);
    };

    screen.Loop(RenderEvent(fn_render, fn_event));
    return EXIT_SUCCESS;
}


UserInterface& UserInterface::instance()
{
    static UserInterface x;
    return x; 
}
