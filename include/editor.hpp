#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <utils.hpp>

namespace editor {

class Program {
public:
    Program();
    ~Program();
    std::string enterTextBuffer(const std::string &name, const std::string &contents);
private:
    std::filesystem::path m_tempdir_path;
}; // class Program

inline Program g_editor_handle;

}; // namespace editor

#endif//EDITOR_HPP
