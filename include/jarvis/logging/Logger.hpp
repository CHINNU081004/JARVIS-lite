#pragma once

#include <iosfwd>
#include <map>
#include <mutex>
#include <string>

namespace jarvis::logging {

enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3
};

std::string toString(LogLevel level);
LogLevel parseLogLevel(const std::string& value);

class Logger {
public:
    explicit Logger(LogLevel level = LogLevel::Info, std::ostream* sink = nullptr);

    void setLevel(LogLevel level);
    LogLevel level() const;

    void debug(const std::string& message, const std::map<std::string, std::string>& fields = {});
    void info(const std::string& message, const std::map<std::string, std::string>& fields = {});
    void warning(const std::string& message, const std::map<std::string, std::string>& fields = {});
    void error(const std::string& message, const std::map<std::string, std::string>& fields = {});

    void log(LogLevel level, const std::string& message, const std::map<std::string, std::string>& fields = {});

private:
    LogLevel level_;
    std::ostream* sink_;
    mutable std::mutex mutex_;
};

} // namespace jarvis::logging

