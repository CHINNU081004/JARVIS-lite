#pragma once

#include "jarvis/config/Config.hpp"
#include "jarvis/logging/Logger.hpp"
#include "jarvis/model/ModelCatalog.hpp"
#include "jarvis/network/Downloader.hpp"
#include "jarvis/platform/SystemInfo.hpp"

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace jarvis::model {

struct ModelAvailability {
    ModelInfo model;
    bool availableForHardware = true;
    std::string reason;
};

struct InstalledModel {
    ModelInfo model;
    std::filesystem::path path;
    bool integrityKnown = false;
    bool integrityVerified = false;
};

using ProgressCallback = std::function<void(std::uint64_t downloaded, std::uint64_t total)>;

class ModelManager {
public:
    ModelManager(config::Config& config,
                 logging::Logger& logger,
                 std::shared_ptr<network::Downloader> downloader = std::make_shared<network::DefaultDownloader>());

    std::vector<ModelAvailability> availableModels(const platform::SystemInfo& system) const;
    std::vector<InstalledModel> installedModels() const;
    std::optional<ModelInfo> activeModel() const;
    std::optional<ModelInfo> recommendedModel(const platform::SystemInfo& system) const;

    bool isInstalled(const std::string& modelId) const;
    std::filesystem::path modelPath(const ModelInfo& model) const;
    std::filesystem::path modelPathForId(const std::string& modelId) const;

    bool selectModel(const std::string& modelId, std::string& message);
    bool removeModel(const std::string& modelId, std::string& message);
    bool verifyModel(const std::string& modelId, std::string& message) const;
    bool downloadModel(const std::string& modelId, std::string& message, ProgressCallback progress = {});
    bool downloadCustomModel(const std::string& modelId,
                             const std::string& url,
                             std::string& message,
                             ProgressCallback progress = {});

private:
    config::Config& config_;
    logging::Logger& logger_;
    std::shared_ptr<network::Downloader> downloader_;
};

} // namespace jarvis::model
