#include <cstdlib>
#include <string>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp> 
#include <ftxui/component/component_options.hpp> 
#include <ftxui/component/screen_interactive.hpp>

#include <utils.hpp>
#include <db.hpp>
#include <editor.hpp>

int main(int argc, char **argv) {
    while (true) {
        auto screen = ftxui::ScreenInteractive::Fullscreen();
        std::vector<std::string> menu_items = {
            "󱧴 Exit",
            " New Entry"
        };
        std::vector<std::string> entry_items = db::g_sqlite_handle.getEntries();
        for_each(entry_items.begin(), entry_items.end(), [&](std::string &s){ menu_items.emplace_back("󰉋 " + s); });

        int selected = 0;
        ftxui::MenuOption option;
        option.on_enter = screen.ExitLoopClosure();
        auto menu = ftxui::Menu(&menu_items, &selected, option);
        screen.Loop(menu);
        
        std::string date, contents;
        switch (selected) {
            case 0:
                exit(0);
            case 1:
                date = utils::get_date();
                break;
            default:
                date = entry_items[selected-2];
                break;
        } 
        contents = db::g_sqlite_handle.getEntry(date);
        std::string output = editor::g_editor_handle.enterTextBuffer(date, contents);
        db::g_sqlite_handle.setEntry(date, output);
    }
}
