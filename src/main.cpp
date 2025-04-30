
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

int main(int argc, char* argv[])
{
    vector<string> items;

    // Check if a filename was provided
    if (argc > 1)
    {
        items = read_lines_from_file(argv[1]);
    }

    // If no items were loaded, use demo items
    if (items.empty()) {
        items = get_demo_items();
    }

    // Create screen and fuzzy finder
    auto screen = ScreenInteractive::FullscreenAlternateScreen();
    auto fuzzy_finder = std::make_shared<UserInterface>(items);

    // Create renderer
    auto renderer = Renderer(fuzzy_finder->Build(), [&fuzzy_finder]
    {
        return fuzzy_finder->Render();
    });

    // Create event handler
    auto event_handler = CatchEvent(renderer, [&fuzzy_finder, &screen](Event event)
    {
        if (event == Event::Escape) {
            screen.Exit();
            return true;
        }
        if (event == Event::Return && fuzzy_finder->GetSelectedIndex() != -1) {
            screen.Exit();
            return true;
        }
        return fuzzy_finder->OnEvent(event);
    });

    screen.Loop(event_handler);

    // Print the selected item
    int selected = fuzzy_finder->GetSelectedIndex();
    if (selected != -1)
    {
        cout << items[selected] << std::endl;
    }

    return 0;
}