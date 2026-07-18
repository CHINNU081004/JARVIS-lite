#pragma once

#include <filesystem>
#include <string>

namespace jarvis::util {

std::string sha256Hex(const std::string& data);
std::string sha256FileHex(const std::filesystem::path& path);

} // namespace jarvis::util

