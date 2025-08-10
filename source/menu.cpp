#include <menu.hpp>

namespace menu {

    int select(std::vector<std::string> &selections) {
        auto screen = ftxui::ScreenInteractive::Fullscreen();
        int selected = 0;
        ftxui::MenuOption option;
        option.on_enter = screen.ExitLoopClosure();
        auto menu = ftxui::Menu(&selections, &selected, option);
        screen.Loop(menu);
        return selected;
    }

} // namespace menu;

