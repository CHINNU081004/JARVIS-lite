#include "jarvis/network/Downloader.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <system_error>

#if defined(JARVIS_HAS_CURL)
#include <curl/curl.h>
#endif

namespace jarvis::network {
namespace {

#if defined(JARVIS_HAS_CURL)
struct CurlContext {
    std::ofstream output;
    std::function<void(std::uint64_t downloaded, std::uint64_t total)> progress;
    curl_off_t existingBytes = 0;
};

std::size_t writeCallback(char* ptr, std::size_t size, std::size_t nmemb, void* userdata) {
    auto* context = static_cast<CurlContext*>(userdata);
    const std::size_t bytes = size * nmemb;
    context->output.write(ptr, static_cast<std::streamsize>(bytes));
    return context->output ? bytes : 0;
}

int progressCallback(void* userdata, curl_off_t total, curl_off_t now, curl_off_t, curl_off_t) {
    auto* context = static_cast<CurlContext*>(userdata);
    if (context->progress) {
        const auto downloaded = static_cast<std::uint64_t>(context->existingBytes + now);
        const auto fullTotal = total > 0 ? static_cast<std::uint64_t>(context->existingBytes + total) : 0;
        context->progress(downloaded, fullTotal);
    }
    return 0;
}
#endif

DownloadResult copyLocalFile(const DownloadRequest& request) {
    std::string source = request.url;
    const std::string prefix = "file://";
    if (source.rfind(prefix, 0) == 0) {
        source = source.substr(prefix.size());
    }

    std::error_code error;
    std::filesystem::create_directories(request.outputPath.parent_path(), error);
    if (error) {
        return {false, "Unable to create output directory: " + error.message(), 0};
    }

    const auto bytes = std::filesystem::file_size(source, error);
    if (error) {
        return {false, "Unable to inspect source file: " + error.message(), 0};
    }

    std::filesystem::copy_file(source, request.outputPath, std::filesystem::copy_options::overwrite_existing, error);
    if (error) {
        return {false, "Unable to copy local file: " + error.message(), 0};
    }

    if (request.progress) {
        request.progress(bytes, bytes);
    }
    return {true, "Downloaded local file", bytes};
}

} // namespace

DownloadResult DefaultDownloader::download(const DownloadRequest& request) {
    if (request.url.empty()) {
        return {false, "No download URL is configured for this model.", 0};
    }

    if (request.url.rfind("file://", 0) == 0 || request.url.find("://") == std::string::npos) {
        return copyLocalFile(request);
    }

#if defined(JARVIS_HAS_CURL)
    std::filesystem::create_directories(request.outputPath.parent_path());

    CurlContext context;
    if (request.resume && std::filesystem::exists(request.outputPath)) {
        context.existingBytes = static_cast<curl_off_t>(std::filesystem::file_size(request.outputPath));
    }

    context.progress = request.progress;
    context.output.open(request.outputPath, std::ios::binary | (context.existingBytes > 0 ? std::ios::app : std::ios::trunc));
    if (!context.output) {
        return {false, "Unable to open output file: " + request.outputPath.string(), 0};
    }

    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
        return {false, "Unable to initialize libcurl.", 0};
    }

    curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &context);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progressCallback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &context);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "jarvis-lite/1.0");

    if (context.existingBytes > 0) {
        curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, context.existingBytes);
    }

    const CURLcode code = curl_easy_perform(curl);
    long responseCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
    curl_easy_cleanup(curl);
    context.output.close();

    const auto finalSize = std::filesystem::exists(request.outputPath) ? std::filesystem::file_size(request.outputPath) : 0;
    if (code != CURLE_OK) {
        return {false, std::string("Download failed: ") + curl_easy_strerror(code), finalSize};
    }
    if (responseCode >= 400) {
        return {false, "Download failed with HTTP status " + std::to_string(responseCode), finalSize};
    }
    return {true, "Download complete", finalSize};
#else
    (void)request;
    return {false, "HTTP downloads require building JARVIS Lite with libcurl support.", 0};
#endif
}

} // namespace jarvis::network

