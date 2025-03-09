#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct AppWidgets {
    Fl_Input* input;
    Fl_Box* output;
};

bool create_script(const char* command)
{
    std::filesystem::path script_path = "permission_test.sh";
    std::ofstream sh_file(script_path);
    if (!sh_file)
    {
        std::cerr << "Error creating file." << std::endl;
        return false;
    }
    sh_file << command;
    sh_file.close();

    std::filesystem::permissions(
        script_path,
        std::filesystem::perms::owner_exec | 
        std::filesystem::perms::owner_read | 
        std::filesystem::perms::owner_write,
        std::filesystem::perm_options::add
    );

    return true;
}

void delete_script()
{
    std::filesystem::path script_path = "permission_test.sh";
    if (std::filesystem::exists(script_path))
    {
        std::filesystem::remove(script_path);
    }
}

bool check_permission(const char* command)
{
    if (!create_script(command))
    {
        return false;
    }

    std::filesystem::path script_path = "permission_test.sh";
    if (!std::filesystem::exists(script_path)) 
    {
        std::cerr << "Script not found." << std::endl;
        return false;
    }

    if ((std::filesystem::status(script_path).permissions() & filesystem::perms::owner_exec) == filesystem::perms::none) {
        std::cerr << "Script is not executable." << std::endl;
        return false;
    }
    return true;
}

string read_from_file(FILE* pipe)
{
    char buffer[128];
    string result = "";
    while (!feof(pipe))
    {
        if (fgets(buffer, 128, pipe) != NULL)
        {
            result += buffer;
        }
    }
    return result;
}

string execute(const char* text)
{
    if (!check_permission(text))
    {
        delete_script();
        return "Error: Permission denied.";
    }

    std::string command = "bash permission_test.sh";
    FILE* pipe = popen(command.c_str(), "r");
    string output = read_from_file(pipe);
    pclose(pipe);

    return output;
}

void button_callback(Fl_Widget* widget, void* data) {
    AppWidgets* widgets = static_cast<AppWidgets*>(data);
    const char* text = widgets->input->value();
    
    string output = execute(text);

    widgets->output->copy_label(output.c_str());
    delete_script();
}


int main() 
{
    Fl_Window* window = new Fl_Window(500, 300, "Simple Console App");

    // x, y, width, height
    Fl_Input* input = new Fl_Input(60, 30, 300, 30, "Console:");
    Fl_Button* button = new Fl_Button(150, 80, 100, 30, "Execute");
    Fl_Box* output = new Fl_Box(50, 130, 300, 30, "Output will appear here");

    AppWidgets widgets = {input, output};
    button->callback(button_callback, &widgets);

    window->end();
    window->show();
    return Fl::run();
}