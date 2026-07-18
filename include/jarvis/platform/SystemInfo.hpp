#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace jarvis::platform {

struct SystemInfo {
    std::string operatingSystem;
    std::string architecture;
    unsigned int cpuCores = 0;
    std::uint64_t totalRamBytes = 0;
    std::uint64_t availableRamBytes = 0;
    std::uint64_t storageAvailableBytes = 0;
    std::filesystem::path homeDirectory;
    std::filesystem::path configDirectory;
    std::filesystem::path dataDirectory;
};

SystemInfo detectSystemInfo(const std::filesystem::path& storageProbePath = {});

std::string formatBytes(std::uint64_t bytes);
bool isArm64(const std::string& architecture);
bool isX64(const std::string& architecture);

} // namespace jarvis::platform

