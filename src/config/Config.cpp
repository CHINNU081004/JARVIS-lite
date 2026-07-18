#include "jarvis/config/Config.hpp"

#include "jarvis/platform/SystemInfo.hpp"
#include "jarvis/util/String.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

namespace jarvis::config {
namespace {

std::string pathString(const std::filesystem::path& path) {
    return path.generic_string();
}

std::string stripQuotes(const std::string& value) {
    auto trimmed = util::trim(value);
    if (trimmed.size() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
        return trimmed.substr(1, trimmed.size() - 2);
    }
    return trimmed;
}

Config withDefaultPaths(Config config) {
    const auto system = platform::detectSystemInfo();
    if (config.modelDirectory.empty()) {
        config.modelDirectory = system.dataDirectory / "models";
    }
    if (config.downloadLocation.empty()) {
        config.downloadLocation = system.dataDirectory / "downloads";
    }
    if (config.threadCount == 0) {
        config.threadCount = system.cpuCores;
    }
    return config;
}

} // namespace

ConfigManager::ConfigManager(std::filesystem::path path)
    : path_(std::move(path)), config_(withDefaultPaths({})) {}

std::filesystem::path ConfigManager::defaultConfigPath() {
    const char* overridePath = std::getenv("JARVIS_LITE_CONFIG");
    if (overridePath != nullptr && *overridePath != '\0') {
        return std::filesystem::path(overridePath);
    }
    return platform::detectSystemInfo().configDirectory / "config.toml";
}

bool ConfigManager::load() {
    if (!std::filesystem::exists(path_)) {
        config_ = withDefaultPaths(config_);
        return false;
    }

    std::ifstream input(path_);
    if (!input) {
        throw std::runtime_error("Unable to read config file: " + path_.string());
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    config_ = withDefaultPaths(parseConfig(buffer.str(), config_));
    return true;
}

void ConfigManager::save() const {
    std::filesystem::create_directories(path_.parent_path());
    std::ofstream output(path_, std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Unable to write config file: " + path_.string());
    }
    output << serializeConfig(config_);
}

Config& ConfigManager::config() {
    return config_;
}

const Config& ConfigManager::config() const {
    return config_;
}

const std::filesystem::path& ConfigManager::path() const {
    return path_;
}

std::string serializeConfig(const Config& config) {
    std::ostringstream out;
    out << "# JARVIS Lite configuration\n";
    out << "active_model = \"" << config.activeModel << "\"\n";
    out << "download_location = \"" << pathString(config.downloadLocation) << "\"\n";
    out << "model_directory = \"" << pathString(config.modelDirectory) << "\"\n";
    out << "thread_count = " << config.threadCount << "\n";
    out << "context_length = " << config.contextLength << "\n";
    out << "logging_level = \"" << logging::toString(config.loggingLevel) << "\"\n";
    return out.str();
}

Config parseConfig(const std::string& content, Config defaults) {
    Config config = std::move(defaults);
    std::istringstream input(content);
    std::string line;
    unsigned int lineNumber = 0;

    while (std::getline(input, line)) {
        ++lineNumber;
        const auto commentPosition = line.find('#');
        if (commentPosition != std::string::npos) {
            line = line.substr(0, commentPosition);
        }
        line = util::trim(line);
        if (line.empty()) {
            continue;
        }

        const auto equalPosition = line.find('=');
        if (equalPosition == std::string::npos) {
            throw std::runtime_error("Invalid config line " + std::to_string(lineNumber));
        }

        const auto key = util::trim(line.substr(0, equalPosition));
        const auto value = stripQuotes(line.substr(equalPosition + 1));

        if (key == "active_model") {
            config.activeModel = value;
        } else if (key == "download_location") {
            config.downloadLocation = value;
        } else if (key == "model_directory") {
            config.modelDirectory = value;
        } else if (key == "thread_count") {
            config.threadCount = static_cast<unsigned int>(std::stoul(value));
        } else if (key == "context_length") {
            config.contextLength = static_cast<unsigned int>(std::stoul(value));
        } else if (key == "logging_level") {
            config.loggingLevel = logging::parseLogLevel(value);
        }
    }

    return config;
}

} // namespace jarvis::config
