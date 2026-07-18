#pragma once

#include <string>
#include <vector>

namespace jarvis::util {

std::string trim(const std::string& value);
std::string toLower(std::string value);
bool startsWith(const std::string& value, const std::string& prefix);
std::vector<std::string> splitWhitespace(const std::string& value);
std::string join(const std::vector<std::string>& values, const std::string& separator);

} // namespace jarvis::util

