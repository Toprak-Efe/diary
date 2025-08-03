#include "sqlite3.h"
#include <db.hpp>

namespace db {

SQLite::SQLite() {
    std::filesystem::path database;
    if (const char *s_home = getenv("HOME")) {
        std::filesystem::path database_dir = std::filesystem::path(s_home) / ".diary";
        if (!std::filesystem::is_directory(database_dir)) {
            std::filesystem::create_directory(database_dir);
        }
        database = database_dir / "diary.sqlite";
    } else {
        std::cerr << "Unable to find HOME, exiting.\n";
        exit(1);
    }
    if (sqlite3_open(database.c_str(), &m_sqlite_handle)) {
        std::cerr << "Unable to start database, SQLite error:\n" << sqlite3_errmsg(m_sqlite_handle) << '\n';
        sqlite3_close(m_sqlite_handle);
        exit(1);
    } 
    char *err = nullptr;
    std::string sql =
        "CREATE TABLE IF NOT EXISTS Diary ("
        "id    INTEGER NOT NULL PRIMARY KEY,"
        "date  TEXT NOT NULL UNIQUE,"
        "entry TEXT"
        ");";
    if (sqlite3_exec(m_sqlite_handle, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "Unable to run SQL, SQLite error:\n" << err << '\n';
        sqlite3_free(err);
        exit(1);
    }
}

SQLite::~SQLite() {
    sqlite3_close(m_sqlite_handle);
}

std::vector<std::string> SQLite::getEntries() {
    std::vector<std::string> results = {};

    sqlite3_stmt *statement = nullptr;
    int rc = sqlite3_prepare_v2(m_sqlite_handle, "SELECT * FROM Diary", -1, &statement, nullptr);
    rc = sqlite3_step(statement);
    
    while (rc==SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(statement, 1);
        results.emplace_back((const char *) text);
        rc = sqlite3_step(statement);
    }
    sqlite3_finalize(statement);
    switch (rc) {
        case SQLITE_DONE:
            return results;
        default:
            std::cerr << "Error running statement. Exiting.\n";
            exit(1);
    }
}

std::string SQLite::getEntry(const std::string &date) {
    sqlite3_stmt *statement = nullptr;
    int rc = sqlite3_prepare_v2(m_sqlite_handle, "SELECT * FROM Diary WHERE date = ?", -1, &statement, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Unable to prepare statement, error.";
        exit(1);
    }
    sqlite3_bind_text(statement, 1, date.c_str(), date.length(), SQLITE_STATIC);
    rc = sqlite3_step(statement);
    switch (rc) {
    case SQLITE_OK:
        break;
    case SQLITE_ROW:
        break;
    case SQLITE_DONE:
        return "";
    default:
        std::cerr << "Unable to execute statement, error.";
        exit(1);
    }
    const unsigned char *text = sqlite3_column_text(statement, 2); 
    std::string entry = (const char *) text;
    return entry;
}

void SQLite::setEntry(const std::string &date, const std::string &entry) {
    std::string sql =
        "INSERT INTO Diary (date, entry)"
        "VALUES (?, ?)"
        "ON CONFLICT (date) DO UPDATE SET"
        "   entry = EXCLUDED.entry";
    sqlite3_stmt *statement = nullptr;
    int rc = sqlite3_prepare_v2(m_sqlite_handle, sql.c_str(), -1, &statement, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Unable to prepare statement, error.";
        exit(1);
    }
    sqlite3_bind_text(statement, 1, date.c_str(), date.length(), SQLITE_STATIC);
    sqlite3_bind_text(statement, 2, entry.c_str(), entry.length(), SQLITE_STATIC);
    rc = sqlite3_step(statement);
    sqlite3_finalize(statement);
    switch (rc) {
        case SQLITE_OK:
            break;
        case SQLITE_DONE:
            break;
        default:
            std::cerr << "Unable to prepare statement, error.";
            exit(1);
    }
}

}; // namespace db

