#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>

namespace jarvis::network {

struct DownloadRequest {
    std::string url;
    std::filesystem::path outputPath;
    bool resume = true;
    std::function<void(std::uint64_t downloaded, std::uint64_t total)> progress;
};

struct DownloadResult {
    bool success = false;
    std::string message;
    std::uint64_t bytesWritten = 0;
};

class Downloader {
public:
    virtual ~Downloader() = default;
    virtual DownloadResult download(const DownloadRequest& request) = 0;
};

class DefaultDownloader final : public Downloader {
public:
    DownloadResult download(const DownloadRequest& request) override;
};

} // namespace jarvis::network

