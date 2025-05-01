
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







vector<string> read_lines_from_file(const string& filename) {
    vector<string> lines;
    ifstream file(filename);
    string line;

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            if (!line.empty())
                lines.push_back(line);
        }
        file.close();
    }

    return lines;
}

vector<string> get_demo_items()
{
    // Return some demo items if file reading fails
    return {
        "app/models/user.rb",
        "app/controllers/users_controller.rb",
        "app/views/users/index.html.erb",
        "app/views/users/show.html.erb",
        "app/views/users/new.html.erb",
        "app/views/users/edit.html.erb",
        "app/models/post.rb",
        "app/controllers/posts_controller.rb",
        "app/views/posts/index.html.erb",
        "app/views/posts/show.html.erb",
        "lib/tasks/data_import.rake",
        "config/routes.rb",
        "config/database.yml",
        "config/application.rb",
        "Gemfile",
        "README.md",
        "Dockerfile",
        ".gitignore",
        "spec/models/user_spec.rb",
        "spec/controllers/users_controller_spec.rb",
        "spec/features/user_login_spec.rb",
        "db/schema.rb",
        "db/migrate/20230401000000_create_users.rb",
        "db/migrate/20230401000001_create_posts.rb",
        "package.json",
        "app/models/user.rb",
        "app/controllers/users_controller.rb",
        "app/views/users/index.html.erb",
        "app/views/users/show.html.erb",
        "app/views/users/new.html.erb",
        "app/views/users/edit.html.erb",
        "app/models/post.rb",
        "app/controllers/posts_controller.rb",
        "app/views/posts/index.html.erb",
        "app/views/posts/show.html.erb",
        "lib/tasks/data_import.rake",
        "config/routes.rb",
        "config/database.yml",
        "config/application.rb",
        "Gemfile",
        "README.md",
        "Dockerfile",
        ".gitignore",
        "spec/models/user_spec.rb",
        "spec/controllers/users_controller_spec.rb",
        "spec/features/user_login_spec.rb",
        "db/schema.rb",
        "db/migrate/20230401000000_create_users.rb",
        "db/migrate/20230401000001_create_posts.rb",
        "package.json",
        "yarn.lock"
    };
}

#if 0
int main(int argc, char** argv)
{
    std::string query;
    auto screen = ScreenInteractive::Fullscreen();

    // Create components with flex_grow to fill available space
    auto right = Renderer([] {
        return text("right") | center | border | flex_grow;
    });

    auto left = Renderer([] {
        return text("left") | center | border | flex_grow;
    });

    auto input_field = Input(&query, "Enter text here...");
    auto input_component = Renderer(input_field, [&] {
        return input_field->Render() | border;
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


int main(int argc, char* argv[])
{
    vector<string> items = get_demo_items();

    // Create screen and fuzzy finder
    auto screen = ScreenInteractive::Fullscreen();
    auto ui = std::make_shared<UserInterface>(items);

    // Create renderer
    auto renderer = Renderer([&ui]
    {
        return ui->Render();
    });

    // Create event handler
    auto event_handler = CatchEvent(renderer, [&ui, &screen](Event event)
    {
        if (event == Event::Escape) {
            screen.Exit();
            return true;
        }
        if (event == Event::Return && ui->GetSelectedIndex() != -1) {
            screen.Exit();
            return true;
        }
        return ui->OnEvent(event);
    });

    screen.Loop(event_handler);

    return EXIT_SUCCESS;
}
