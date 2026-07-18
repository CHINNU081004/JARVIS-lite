#include "jarvis/util/String.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace jarvis::util {

std::string trim(const std::string& value) {
    auto begin = value.begin();
    while (begin != value.end() && std::isspace(static_cast<unsigned char>(*begin)) != 0) {
        ++begin;
    }

    auto end = value.end();
    while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1))) != 0) {
        --end;
    }

    return std::string(begin, end);
}

std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

bool startsWith(const std::string& value, const std::string& prefix) {
    return value.rfind(prefix, 0) == 0;
}

std::vector<std::string> splitWhitespace(const std::string& value) {
    std::istringstream input(value);
    std::vector<std::string> parts;
    std::string part;
    while (input >> part) {
        parts.push_back(part);
    }
    return parts;
}

std::string join(const std::vector<std::string>& values, const std::string& separator) {
    std::ostringstream out;
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            out << separator;
        }
        out << values[i];
    }
    return out.str();
}

} // namespace jarvis::util

