#include "menu.hpp"
#include <cstdlib>
#include <string>

#include <utils.hpp>
#include <db.hpp>
#include <editor.hpp>

int main(int argc, char **argv) {
    auto agenda_menu = [&]() {
        std::string contents;
        contents = db::g_sqlite_handle.getEntry("0");
        std::string output = editor::g_editor_handle.enterTextBuffer("0", contents);
        db::g_sqlite_handle.setEntry("0", output);
    };
    auto diary_menu = [&]() {
        while (true) {
            std::vector<std::string> menu_items = {
                " Back",
                " Addition"
            }; 
            std::vector<std::string> entry_items = db::g_sqlite_handle.getEntries();
            for_each(entry_items.begin(), entry_items.end(), [&](std::string &s){ if (s != "0") menu_items.emplace_back("󰉋 " + s); });
            int selected = menu::select(menu_items);
            std::string date, contents;
            switch (selected) {
                case 0:
                    return;
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
    };
    auto config_menu = [&]() {
        while (true) {
            std::vector<std::string> menu_items = {
                " Back",
                " Delete"
            };
            int selected = menu::select(menu_items);
            switch (selected) {
                case 0:
                    return;
                case 1:
                    db::g_sqlite_handle.purgeDatabase();
                    break;
            }
        }
    };
    auto main_menu = [&]() {
        while (true) {
            std::vector<std::string> menu_items = {
                "󱧴 Exit",
                "󰂺 Agenda",
                " Diary",
                "󱁿 Config",
            };
            int selected = menu::select(menu_items);
            switch (selected) {
                case 0:
                    exit(0);
                case 1:
                    agenda_menu();
                    break;
                case 2:
                    diary_menu();
                    break;
                case 3:
                    config_menu();
                    break;
                default:
                    break;
            }
        }
    };

    main_menu();
}
