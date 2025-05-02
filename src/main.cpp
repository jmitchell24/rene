
//
// rene
//
#include "user_interface.hpp"
using namespace rene;

//
// std
//
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;


//
// ftxui
//
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component_options.hpp>
using namespace ftxui;










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

static Elements createItemElements(vector<string> const& items, int selected_index)
{
    Elements el;
    for (size_t i = 0; i < items.size(); ++i)
    {
        bool is_selected = (i == selected_index);
        auto&& it = items[i];

        if (is_selected)
        {
            el.push_back(hbox({
                text("> ") | color(Color::Green) | bold,
                text(it) | inverted
            }) | notflex | focus);
        }
        else
        {
            el.push_back(hbox({
                text("  "),
                text(it)
            }) | notflex);
        }
    }


    return el;
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

int main(int argc, char** argv)
{
    auto screen = ScreenInteractive::Fullscreen();

    // Create the list components
    auto right = Renderer([] {
        return vbox(createItemElements(g_list_right, g_selected_index)) | frame | border | notflex;
    });

    auto left = Renderer([] {
        return vbox(createItemElements(g_list_left, g_selected_index)) | vscroll_indicator | frame | border | notflex;
    });

    // Create the input component with fixed height
    auto input_field = Input(&g_query, "asd");
    auto input_component = Renderer(input_field, [&] {
        return input_field->Render() | border;
    });

    // Initialize split sizes
    int left_size = 20;

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

#if 0
int main(int argc, char** argv)
{
    auto screen = ScreenInteractive::Fullscreen();

    // Create components with flex_grow to fill available space
    auto right = Renderer([] {
        return vbox(createItemElements(g_list_right)) | vscroll_indicator | yframe | border;
        //return text("right") | center | frame;
    });

    auto left = Renderer([] {
        return vbox(createItemElements(g_list_left)) | vscroll_indicator | yframe | border ;
        //return text("left") | center | frame;
    });

    auto input_field = Input(&g_query, "asd");
    auto input_component = Renderer(input_field, [&] {
        return input_field->Render() | border | notflex;
    });

    // Initialize split sizes
    int left_size = 20;
    int right_size = 20;

    // Create a resizable split that will grow to fill available space
    auto split = ResizableSplitLeft(left, right, &left_size);
    auto split_with_flex = Renderer(split, [&] {
        return split->Render() | flex_grow;
    });

    // Create the main container with the split on top and input at bottom
    auto container = Container::Vertical({
        split_with_flex,
        input_component
    });

    // Create the final renderer
    auto renderer = Renderer(container, [&] {
        return container->Render();
    });

    screen.Loop(renderer);
    return EXIT_SUCCESS;
}
#endif


#if 0
int main(int argc, char* argv[])
{
    vector<string> items = get_demo_items();

    // Create screen and fuzzy finder
    auto screen = ScreenInteractive::Fullscreen();

    // Create renderer
    auto ui = Make<UserInterface>(screen.ExitLoopClosure());

    // Create event handler
    // auto event_handler = CatchEvent(renderer, [&ui, &screen](Event event)
    // {
    //     if (event == Event::Escape) {
    //         screen.Exit();
    //         return true;
    //     }
    //     if (event == Event::Return && ui->GetSelectedIndex() != -1) {
    //         screen.Exit();
    //         return true;
    //     }
    //     return ui->OnEvent(event);
    // });

    screen.Loop(ui);

    return EXIT_SUCCESS;
}

#endif