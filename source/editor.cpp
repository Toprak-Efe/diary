#include <editor.hpp>

namespace editor {

Program::Program() {
    std::filesystem::path temp_template = std::filesystem::temp_directory_path() / "diary.XXXXXX";
    std::string temp_template_string = temp_template.string();
    char *temp_template_str = temp_template_string.data();
    const char *directory_temp_name = mkdtemp(temp_template_str);
}

Program::~Program() {
    std::filesystem::remove_all(m_tempdir_path);
}

std::string Program::enterTextBuffer(const std::string &name, const std::string &contents) {
    tm time = utils::get_tm();
    std::filesystem::path temp_file_path = m_tempdir_path / std::format("{:02}{:02}{:02}{:02}{:02}.md",
        time.tm_year+1900, time.tm_mday+1, time.tm_hour, time.tm_min, time.tm_sec
    );
    std::fstream temp_file;
    temp_file.open(temp_file_path, std::fstream::in | std::fstream::out | std::fstream::app);
    if (!temp_file) {
        std::cerr << "Unable to open buffer file, exiting.\n";
        exit(1);
    }
    std::string header = std::format("# Entry at {:02} {}\n", time.tm_hour % 12, (time.tm_hour <= 12 ? "AM" : "PM"));
    temp_file << contents;
    if (!contents.contains(header)) {
        if (contents.length() != 0)
            temp_file << '\n';
        temp_file << header << "\n\n";
    }
    flush(temp_file);
    std::string command = format("nvim +$ {}", temp_file_path.string());
    system(command.c_str());
    std::string output = utils::slurp(temp_file_path);
    temp_file.close();
    std::filesystem::remove(temp_file_path);
    return output;
}

}; // namespace editor
