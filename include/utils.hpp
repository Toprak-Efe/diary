#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <chrono>
#include <format>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace utils {

tm get_tm();
std::string get_timestamp();
std::string get_date();
std::string slurp (const std::string& path);

}; // namespace utils

#endif//UTILS_HPP
