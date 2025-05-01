#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <format>
#include <filesystem>
#include <chrono>
#include <sqlite3.h>

using namespace std::filesystem;
using namespace std::chrono;
using namespace std;

static sqlite3 *g_sqlite_handle = nullptr;
static string   g_buffer;
static bool     g_found = false;

inline tm get_tm() {
    system_clock::time_point now = system_clock::now();
    time_t tt = system_clock::to_time_t(now);
    return *localtime(&tt);
}

inline string get_date(const tm &local) {
    stringstream date_s;
    date_s << local.tm_year+1900 << format("{:02}", local.tm_mon+1) <<  format("{:02}", local.tm_mday);
    return date_s.str();
}

void sql_cleanup() {
    sqlite3_close(g_sqlite_handle);
}

bool sql_find_entry(const string &date) {
    bool out = false;
    sqlite3_stmt *statement = nullptr;
    int rc = sqlite3_prepare_v2(g_sqlite_handle, "SELECT entry FROM Diary WHERE date = ?", -1, &statement, nullptr);
    sqlite3_bind_text(statement, 1, date.c_str(), date.length(), SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        cerr << "Unable to prepare statement, error.";
        exit(1);
    }
    rc = sqlite3_step(statement);
    switch (rc) {
        case SQLITE_ROW:
            {
            const unsigned char *text = sqlite3_column_text(statement, 0);
            g_buffer = (const char *) text;
            }
            out = true;
            break;
        case SQLITE_DONE:
            out = false;
            g_buffer = "";
            break;
        default:
            cerr << "Error running statement. Exiting.\n";
            sqlite3_finalize(statement);
            exit(1);
            break;
    }
    sqlite3_finalize(statement);
    return out;
}

void sql_insert_entry(const string &date) {
    sqlite3_stmt *statement = nullptr;
    int rc = sqlite3_prepare_v2(g_sqlite_handle, "INSERT INTO Diary (date, entry) VALUES (?, NULL)", -1, &statement, nullptr);
    sqlite3_bind_text(statement, 1, date.c_str(), date.length(), SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        cerr << "Unable to prepare statement, error.\n";
        exit(1);
    }
    rc = sqlite3_step(statement);
    if (rc != SQLITE_DONE) {
        cerr << "Unable to execute statement, error.\n";
        exit(1);
    }
    sqlite3_finalize(statement);
}

void sql_initalize_connection() {
    path database;
    if (const char *s_home = getenv("HOME")) {
        path database_dir = path(s_home) / ".diary";
        if (!is_directory(database_dir)) {
            create_directory(database_dir);
        }
        database = database_dir / "diary.sqlite";
    } else {
        cerr << "Unable to find HOME, exiting.\n";
        exit(1);
    }
    if (sqlite3_open(database.c_str(), &g_sqlite_handle)) {
        cerr << "Unable to start database, SQLite error:\n" << sqlite3_errmsg(g_sqlite_handle) << '\n';
        sqlite3_close(g_sqlite_handle);
        exit(1);
    } else {
        atexit(&sql_cleanup);
    }
}

void sql_initialize_table() {
    char *err = nullptr;
    string sql =
        "CREATE TABLE IF NOT EXISTS Diary ("
        "id    INTEGER NOT NULL PRIMARY KEY,"
        "date  TEXT,"
        "entry TEXT"
        ");";
    if (sqlite3_exec(g_sqlite_handle, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        cerr << "Unable to run SQL, SQLite error:\n" << err << '\n';
        sqlite3_free(err);
        exit(1);
    }
}

void sql_update_entry(const string &date) {
    sqlite3_stmt *statement = nullptr;
    int rc = sqlite3_prepare_v2(g_sqlite_handle, "UPDATE Diary SET entry = ? WHERE date = ?", -1, &statement, nullptr);
    sqlite3_bind_text(statement, 1, g_buffer.c_str(), g_buffer.length(), SQLITE_STATIC);
    sqlite3_bind_text(statement, 2, date.c_str(), date.length(), SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        cerr << "Unable to prepare statement, error.\n";
        exit(1);
    }
    rc = sqlite3_step(statement);
    if (rc != SQLITE_DONE) {
        cerr << "Unable to execute statement, error.\n";
        exit(1);
    }
    sqlite3_finalize(statement);
}

inline string slurp (const string& path) {
    ostringstream buf; 
    ifstream input (path.c_str()); 
    buf << input.rdbuf(); 
    return buf.str();
}

void nvim_initialize_runtime(const string &date, const tm &time) {
    path temp_file_path = temp_directory_path() / std::format("{}.md", date);
    fstream temp_file;
    temp_file.open(temp_file_path, fstream::in | fstream::out | fstream::app);
    if (!temp_file) {
        cerr << "Unable to open buffer file, exiting.\n";
        exit(1);
    }
    string header = format("# Entry at {:02} {}\n", time.tm_hour % 12, (time.tm_hour <= 12 ? "AM" : "PM"));
    temp_file << g_buffer;
    if (!g_buffer.contains(header)) {
        if (g_buffer.length() != 0)
            temp_file << '\n';
        temp_file << header << "\n\n";
    }
    flush(temp_file);
    string command = format("nvim +$ {}", temp_file_path.string());
    system(command.c_str());
    g_buffer = slurp(temp_file_path);
    temp_file.close();
    filesystem::remove(temp_file_path);
}

int main(int argc, char **argv) {
    sql_initalize_connection();
    sql_initialize_table();

    tm local = get_tm();
    string date = get_date(local);
    
    g_found = sql_find_entry(date);
    if (!g_found) {
        sql_insert_entry(date);
    }
    
    nvim_initialize_runtime(date, local);
    sql_update_entry(date);
    exit(0);
}

