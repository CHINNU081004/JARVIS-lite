#include "jarvis/agent/FileAgent.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <system_error>

namespace jarvis::agent {

AgentResult FileAgent::readFile(const std::filesystem::path& path, std::string& content) const {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        return {false, "Unable to read file: " + path.string()};
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    content = buffer.str();
    return {true, "Read file: " + path.string()};
}

AgentResult FileAgent::writeFile(const std::filesystem::path& path, const std::string& content) const {
    std::error_code error;
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path(), error);
        if (error) {
            return {false, "Unable to create parent directory: " + error.message()};
        }
    }

    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) {
        return {false, "Unable to write file: " + path.string()};
    }
    output << content;
    return {true, "Wrote file: " + path.string()};
}

AgentResult FileAgent::createDirectory(const std::filesystem::path& path) const {
    std::error_code error;
    std::filesystem::create_directories(path, error);
    if (error) {
        return {false, "Unable to create directory: " + error.message()};
    }
    return {true, "Created directory: " + path.string()};
}

AgentResult FileAgent::renamePath(const std::filesystem::path& from, const std::filesystem::path& to) const {
    std::error_code error;
    std::filesystem::rename(from, to, error);
    if (error) {
        return {false, "Unable to rename path: " + error.message()};
    }
    return {true, "Renamed " + from.string() + " to " + to.string()};
}

AgentResult FileAgent::copyPath(const std::filesystem::path& from, const std::filesystem::path& to) const {
    std::error_code error;
    if (std::filesystem::is_directory(from)) {
        std::filesystem::copy(from, to, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, error);
    } else {
        if (to.has_parent_path()) {
            std::filesystem::create_directories(to.parent_path(), error);
            if (error) {
                return {false, "Unable to create destination directory: " + error.message()};
            }
        }
        std::filesystem::copy_file(from, to, std::filesystem::copy_options::overwrite_existing, error);
    }
    if (error) {
        return {false, "Unable to copy path: " + error.message()};
    }
    return {true, "Copied " + from.string() + " to " + to.string()};
}

AgentResult FileAgent::movePath(const std::filesystem::path& from, const std::filesystem::path& to) const {
    auto renamed = renamePath(from, to);
    if (renamed.success) {
        return renamed;
    }
    auto copied = copyPath(from, to);
    if (!copied.success) {
        return copied;
    }
    std::error_code error;
    std::filesystem::remove_all(from, error);
    if (error) {
        return {false, "Copied but failed to remove original: " + error.message()};
    }
    return {true, "Moved " + from.string() + " to " + to.string()};
}

AgentResult FileAgent::listDirectory(const std::filesystem::path& path, std::vector<DirectoryEntry>& entries) const {
    std::error_code error;
    if (!std::filesystem::exists(path, error)) {
        return {false, "Directory does not exist: " + path.string()};
    }
    if (!std::filesystem::is_directory(path, error)) {
        return {false, "Path is not a directory: " + path.string()};
    }

    entries.clear();
    for (const auto& entry : std::filesystem::directory_iterator(path, error)) {
        if (error) {
            return {false, "Unable to list directory: " + error.message()};
        }
        const bool isDir = entry.is_directory(error);
        std::uintmax_t size = 0;
        if (!isDir) {
            size = entry.file_size(error);
            if (error) {
                size = 0;
                error.clear();
            }
        }
        entries.push_back({entry.path(), isDir, size});
    }
    return {true, "Listed directory: " + path.string()};
}

AgentResult FileAgent::executeCommand(const std::string& command, int& exitCode) const {
    if (command.empty()) {
        return {false, "Command is empty"};
    }
    exitCode = std::system(command.c_str());
    return {exitCode == 0, "Command exited with code " + std::to_string(exitCode)};
}

} // namespace jarvis::agent

