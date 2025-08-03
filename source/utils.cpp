#include <utils.hpp>

namespace utils {

tm get_tm() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    return *localtime(&tt);
}

std::string get_timestamp() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm tm_now = *localtime(&tt);
    std::stringstream date_s;
    date_s
        << tm_now.tm_year+1900
        << std::format("{:02}", tm_now.tm_mon+1)
        << std::format("{:02}", tm_now.tm_mday)
        << std::format("{:02}", tm_now.tm_hour)
        << std::format("{:02}", tm_now.tm_min)
        << std::format("{:02}", tm_now.tm_sec);
    return date_s.str();
}

std::string get_date() {
    const tm local = get_tm();
    std::stringstream date_s;
    date_s << local.tm_year+1900 << std::format("{:02}", local.tm_mon+1) <<  std::format("{:02}", local.tm_mday);
    return date_s.str();
}

std::string slurp (const std::string& path) {
    std::ostringstream buf; 
    std::ifstream input (path.c_str()); 
    buf << input.rdbuf(); 
    return buf.str();
}

}; // namespace utils
