#ifndef DB_HPP
#define DB_HPP

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <sqlite3.h>

#include <utils.hpp>

namespace db {

class SQLite {
public:
    SQLite();
    ~SQLite();
    std::vector<std::string> getEntries();
    std::string getEntry(const std::string &date);
    void setEntry(const std::string &date, const std::string &entry);
private:
    sqlite3 *m_sqlite_handle;
};

inline SQLite g_sqlite_handle;

}; // namespace db

#endif//DB_HPP
