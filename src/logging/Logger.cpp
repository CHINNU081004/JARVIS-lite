#include "jarvis/logging/Logger.hpp"

#include "jarvis/util/String.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace jarvis::logging {
namespace {

std::string escapeJson(const std::string& value) {
    std::ostringstream out;
    for (const char ch : value) {
        switch (ch) {
        case '\\':
            out << "\\\\";
            break;
        case '"':
            out << "\\\"";
            break;
        case '\n':
            out << "\\n";
            break;
        case '\r':
            out << "\\r";
            break;
        case '\t':
            out << "\\t";
            break;
        default:
            out << ch;
            break;
        }
    }
    return out.str();
}

std::string isoTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm {};
#if defined(_WIN32)
    gmtime_s(&tm, &time);
#else
    gmtime_r(&time, &tm);
#endif
    std::ostringstream out;
    out << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return out.str();
}

} // namespace

std::string toString(LogLevel level) {
    switch (level) {
    case LogLevel::Debug:
        return "debug";
    case LogLevel::Info:
        return "info";
    case LogLevel::Warning:
        return "warning";
    case LogLevel::Error:
        return "error";
    }
    return "info";
}

LogLevel parseLogLevel(const std::string& value) {
    const auto normalized = util::toLower(util::trim(value));
    if (normalized == "debug") {
        return LogLevel::Debug;
    }
    if (normalized == "info") {
        return LogLevel::Info;
    }
    if (normalized == "warning" || normalized == "warn") {
        return LogLevel::Warning;
    }
    if (normalized == "error") {
        return LogLevel::Error;
    }
    throw std::invalid_argument("Unsupported log level: " + value);
}

Logger::Logger(LogLevel level, std::ostream* sink)
    : level_(level), sink_(sink == nullptr ? &std::clog : sink) {}

void Logger::setLevel(LogLevel level) {
    std::lock_guard lock(mutex_);
    level_ = level;
}

LogLevel Logger::level() const {
    std::lock_guard lock(mutex_);
    return level_;
}

void Logger::debug(const std::string& message, const std::map<std::string, std::string>& fields) {
    log(LogLevel::Debug, message, fields);
}

void Logger::info(const std::string& message, const std::map<std::string, std::string>& fields) {
    log(LogLevel::Info, message, fields);
}

void Logger::warning(const std::string& message, const std::map<std::string, std::string>& fields) {
    log(LogLevel::Warning, message, fields);
}

void Logger::error(const std::string& message, const std::map<std::string, std::string>& fields) {
    log(LogLevel::Error, message, fields);
}

void Logger::log(LogLevel messageLevel, const std::string& message, const std::map<std::string, std::string>& fields) {
    std::lock_guard lock(mutex_);
    if (static_cast<int>(messageLevel) < static_cast<int>(level_)) {
        return;
    }

    *sink_ << "{\"ts\":\"" << isoTimestamp()
           << "\",\"level\":\"" << toString(messageLevel)
           << "\",\"message\":\"" << escapeJson(message) << "\"";

    for (const auto& [key, value] : fields) {
        *sink_ << ",\"" << escapeJson(key) << "\":\"" << escapeJson(value) << "\"";
    }

    *sink_ << "}" << std::endl;
}

} // namespace jarvis::logging

