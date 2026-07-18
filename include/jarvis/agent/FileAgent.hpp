#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace jarvis::agent {

struct AgentResult {
    bool success = false;
    std::string message;
};

struct DirectoryEntry {
    std::filesystem::path path;
    bool isDirectory = false;
    std::uintmax_t size = 0;
};

class FileAgent {
public:
    AgentResult readFile(const std::filesystem::path& path, std::string& content) const;
    AgentResult writeFile(const std::filesystem::path& path, const std::string& content) const;
    AgentResult createDirectory(const std::filesystem::path& path) const;
    AgentResult renamePath(const std::filesystem::path& from, const std::filesystem::path& to) const;
    AgentResult copyPath(const std::filesystem::path& from, const std::filesystem::path& to) const;
    AgentResult movePath(const std::filesystem::path& from, const std::filesystem::path& to) const;
    AgentResult listDirectory(const std::filesystem::path& path, std::vector<DirectoryEntry>& entries) const;
    AgentResult executeCommand(const std::string& command, int& exitCode) const;
};

} // namespace jarvis::agent

