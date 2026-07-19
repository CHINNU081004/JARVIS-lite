#pragma once

#include "jarvis/logging/Logger.hpp"

#include <filesystem>
#include <string>

namespace jarvis::config {

struct Config {
    std::string activeModel = "gemma-4-e2b-it-q4";
    std::filesystem::path downloadLocation;
    std::filesystem::path modelDirectory;
    unsigned int threadCount = 0;
    unsigned int contextLength = 4096;
    logging::LogLevel loggingLevel = logging::LogLevel::Info;
};

class ConfigManager {
public:
    explicit ConfigManager(std::filesystem::path path);

    static std::filesystem::path defaultConfigPath();

    bool load();
    void save() const;

    Config& config();
    const Config& config() const;
    const std::filesystem::path& path() const;

private:
    std::filesystem::path path_;
    Config config_;
};

std::string serializeConfig(const Config& config);
Config parseConfig(const std::string& content, Config defaults = {});

} // namespace jarvis::config
