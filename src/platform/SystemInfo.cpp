#include "jarvis/platform/SystemInfo.hpp"

#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <thread>

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <sys/sysctl.h>
#elif defined(__linux__)
#include <sys/sysinfo.h>
#endif

namespace jarvis::platform {
namespace {

std::filesystem::path envPath(const char* name) {
    const char* value = std::getenv(name);
    if (value == nullptr || *value == '\0') {
        return {};
    }
    return std::filesystem::path(value);
}

std::filesystem::path homeDirectory() {
#if defined(_WIN32)
    auto profile = envPath("USERPROFILE");
    if (!profile.empty()) {
        return profile;
    }
    auto drive = envPath("HOMEDRIVE");
    auto path = envPath("HOMEPATH");
    if (!drive.empty() && !path.empty()) {
        return drive / path;
    }
#else
    auto home = envPath("HOME");
    if (!home.empty()) {
        return home;
    }
#endif
    return std::filesystem::current_path();
}

std::string osName() {
#if defined(_WIN32)
    return "Windows";
#elif defined(__APPLE__)
    return "macOS";
#elif defined(__linux__)
    return "Linux";
#else
    return "Unknown";
#endif
}

std::string archName() {
#if defined(__x86_64__) || defined(_M_X64)
    return "x64";
#elif defined(__aarch64__) || defined(_M_ARM64)
    return "arm64";
#elif defined(__i386__) || defined(_M_IX86)
    return "x86";
#elif defined(__arm__) || defined(_M_ARM)
    return "arm";
#else
    return "unknown";
#endif
}

std::uint64_t totalRam() {
#if defined(_WIN32)
    MEMORYSTATUSEX status {};
    status.dwLength = sizeof(status);
    if (GlobalMemoryStatusEx(&status) != 0) {
        return static_cast<std::uint64_t>(status.ullTotalPhys);
    }
#elif defined(__APPLE__)
    std::uint64_t value = 0;
    std::size_t size = sizeof(value);
    if (sysctlbyname("hw.memsize", &value, &size, nullptr, 0) == 0) {
        return value;
    }
#elif defined(__linux__)
    struct sysinfo info {};
    if (sysinfo(&info) == 0) {
        return static_cast<std::uint64_t>(info.totalram) * info.mem_unit;
    }
#endif
    return 0;
}

std::uint64_t availableRam() {
#if defined(_WIN32)
    MEMORYSTATUSEX status {};
    status.dwLength = sizeof(status);
    if (GlobalMemoryStatusEx(&status) != 0) {
        return static_cast<std::uint64_t>(status.ullAvailPhys);
    }
#elif defined(__APPLE__)
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vmstat {};
    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, reinterpret_cast<host_info64_t>(&vmstat), &count) == KERN_SUCCESS) {
        vm_size_t pageSize = 0;
        host_page_size(mach_host_self(), &pageSize);
        const auto freePages = static_cast<std::uint64_t>(vmstat.free_count + vmstat.inactive_count);
        return freePages * static_cast<std::uint64_t>(pageSize);
    }
#elif defined(__linux__)
    struct sysinfo info {};
    if (sysinfo(&info) == 0) {
        return static_cast<std::uint64_t>(info.freeram + info.bufferram) * info.mem_unit;
    }
#endif
    return 0;
}

std::filesystem::path configDirectory(const std::filesystem::path& home) {
#if defined(_WIN32)
    auto appData = envPath("APPDATA");
    return (appData.empty() ? home / "AppData" / "Roaming" : appData) / "JARVIS Lite";
#elif defined(__APPLE__)
    return home / "Library" / "Application Support" / "JARVIS Lite";
#else
    auto xdg = envPath("XDG_CONFIG_HOME");
    return (xdg.empty() ? home / ".config" : xdg) / "jarvis-lite";
#endif
}

std::filesystem::path dataDirectory(const std::filesystem::path& home) {
#if defined(_WIN32)
    auto local = envPath("LOCALAPPDATA");
    return (local.empty() ? home / "AppData" / "Local" : local) / "JARVIS Lite";
#elif defined(__APPLE__)
    return home / "Library" / "Application Support" / "JARVIS Lite";
#else
    auto xdg = envPath("XDG_DATA_HOME");
    return (xdg.empty() ? home / ".local" / "share" : xdg) / "jarvis-lite";
#endif
}

} // namespace

SystemInfo detectSystemInfo(const std::filesystem::path& storageProbePath) {
    const auto home = homeDirectory();
    const auto dataDir = dataDirectory(home);
    const auto probe = storageProbePath.empty() ? (std::filesystem::exists(dataDir) ? dataDir : home) : storageProbePath;

    std::uint64_t storageAvailable = 0;
    try {
        storageAvailable = std::filesystem::space(probe).available;
    } catch (...) {
        storageAvailable = 0;
    }

    return SystemInfo {
        .operatingSystem = osName(),
        .architecture = archName(),
        .cpuCores = std::max(1u, std::thread::hardware_concurrency()),
        .totalRamBytes = totalRam(),
        .availableRamBytes = availableRam(),
        .storageAvailableBytes = storageAvailable,
        .homeDirectory = home,
        .configDirectory = configDirectory(home),
        .dataDirectory = dataDir
    };
}

std::string formatBytes(std::uint64_t bytes) {
    static constexpr const char* units[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    double value = static_cast<double>(bytes);
    std::size_t unitIndex = 0;
    while (value >= 1024.0 && unitIndex < 4) {
        value /= 1024.0;
        ++unitIndex;
    }

    std::ostringstream out;
    if (unitIndex == 0) {
        out << static_cast<std::uint64_t>(value) << ' ' << units[unitIndex];
    } else {
        out.setf(std::ios::fixed);
        out.precision(1);
        out << value << ' ' << units[unitIndex];
    }
    return out.str();
}

bool isArm64(const std::string& architecture) {
    return architecture == "arm64";
}

bool isX64(const std::string& architecture) {
    return architecture == "x64";
}

} // namespace jarvis::platform

