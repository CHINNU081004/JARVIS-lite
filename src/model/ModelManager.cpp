#include "jarvis/model/ModelManager.hpp"

#include "jarvis/platform/SystemInfo.hpp"
#include "jarvis/util/Sha256.hpp"

#include <algorithm>
#include <filesystem>
#include <sstream>

namespace jarvis::model {
namespace {

std::string hardwareReason(const ModelInfo& model, const platform::SystemInfo& system) {
    if (system.totalRamBytes > 0 && model.recommendedRamBytes > system.totalRamBytes) {
        return "requires about " + platform::formatBytes(model.recommendedRamBytes)
            + " RAM, system has " + platform::formatBytes(system.totalRamBytes);
    }
    if (system.storageAvailableBytes > 0 && model.sizeBytes > system.storageAvailableBytes) {
        return "requires about " + platform::formatBytes(model.sizeBytes)
            + " storage, available storage is " + platform::formatBytes(system.storageAvailableBytes);
    }
    return {};
}

} // namespace

ModelManager::ModelManager(config::Config& config,
                           logging::Logger& logger,
                           std::shared_ptr<network::Downloader> downloader)
    : config_(config), logger_(logger), downloader_(std::move(downloader)) {}

std::vector<ModelAvailability> ModelManager::availableModels(const platform::SystemInfo& system) const {
    std::vector<ModelAvailability> result;
    for (const auto& model : ModelCatalog::builtInModels()) {
        auto reason = hardwareReason(model, system);
        result.push_back({
            .model = model,
            .availableForHardware = reason.empty(),
            .reason = std::move(reason)
        });
    }
    return result;
}

std::vector<InstalledModel> ModelManager::installedModels() const {
    std::vector<InstalledModel> result;
    for (const auto& model : ModelCatalog::builtInModels()) {
        const auto path = modelPath(model);
        if (!std::filesystem::exists(path)) {
            continue;
        }

        const bool hasHash = !model.sha256.empty();
        bool verified = false;
        if (hasHash) {
            try {
                verified = util::sha256FileHex(path) == model.sha256;
            } catch (...) {
                verified = false;
            }
        }

        result.push_back({
            .model = model,
            .path = path,
            .integrityKnown = hasHash,
            .integrityVerified = verified
        });
    }
    return result;
}

std::optional<ModelInfo> ModelManager::activeModel() const {
    return ModelCatalog::findById(config_.activeModel);
}

std::optional<ModelInfo> ModelManager::recommendedModel(const platform::SystemInfo& system) const {
    auto available = availableModels(system);
    available.erase(std::remove_if(available.begin(), available.end(), [](const ModelAvailability& item) {
        return !item.availableForHardware || item.model.category != ModelCategory::GeneralAssistant;
    }), available.end());

    if (available.empty()) {
        return std::nullopt;
    }

    std::sort(available.begin(), available.end(), [](const ModelAvailability& left, const ModelAvailability& right) {
        return left.model.recommendedRamBytes > right.model.recommendedRamBytes;
    });
    return available.front().model;
}

bool ModelManager::isInstalled(const std::string& modelId) const {
    const auto model = ModelCatalog::findById(modelId);
    return model.has_value() && std::filesystem::exists(modelPath(*model));
}

std::filesystem::path ModelManager::modelPath(const ModelInfo& model) const {
    return config_.modelDirectory / (model.id + ".gguf");
}

bool ModelManager::selectModel(const std::string& modelId, std::string& message) {
    const auto model = ModelCatalog::findById(modelId);
    if (!model) {
        message = "Unknown model: " + modelId;
        return false;
    }

    if (!isInstalled(modelId)) {
        message = "Model is known but not installed: " + modelId;
        return false;
    }

    config_.activeModel = modelId;
    message = "Active model set to " + model->name;
    logger_.info("Active model changed", {{"model_id", modelId}});
    return true;
}

bool ModelManager::removeModel(const std::string& modelId, std::string& message) {
    const auto model = ModelCatalog::findById(modelId);
    if (!model) {
        message = "Unknown model: " + modelId;
        return false;
    }

    const auto path = modelPath(*model);
    if (!std::filesystem::exists(path)) {
        message = "Model is not installed: " + modelId;
        return false;
    }

    std::error_code error;
    std::filesystem::remove(path, error);
    if (error) {
        message = "Unable to remove model: " + error.message();
        return false;
    }

    message = "Removed model: " + model->name;
    logger_.info("Model removed", {{"model_id", modelId}});
    return true;
}

bool ModelManager::verifyModel(const std::string& modelId, std::string& message) const {
    const auto model = ModelCatalog::findById(modelId);
    if (!model) {
        message = "Unknown model: " + modelId;
        return false;
    }
    const auto path = modelPath(*model);
    if (!std::filesystem::exists(path)) {
        message = "Model is not installed: " + modelId;
        return false;
    }
    if (model->sha256.empty()) {
        message = "No SHA-256 checksum is configured for " + modelId + ". File exists at " + path.string();
        return true;
    }

    const auto actual = util::sha256FileHex(path);
    if (actual != model->sha256) {
        message = "Integrity check failed for " + modelId;
        return false;
    }

    message = "Integrity check passed for " + modelId;
    return true;
}

bool ModelManager::downloadModel(const std::string& modelId, std::string& message, ProgressCallback progress) {
    const auto model = ModelCatalog::findById(modelId);
    if (!model) {
        message = "Unknown model: " + modelId;
        return false;
    }

    std::filesystem::create_directories(config_.modelDirectory);
    const auto result = downloader_->download({
        .url = model->downloadUrl,
        .outputPath = modelPath(*model),
        .resume = true,
        .progress = std::move(progress)
    });

    if (!result.success) {
        message = result.message;
        logger_.warning("Model download failed", {{"model_id", modelId}, {"reason", result.message}});
        return false;
    }

    if (!model->sha256.empty()) {
        std::string verifyMessage;
        if (!verifyModel(modelId, verifyMessage)) {
            message = verifyMessage;
            return false;
        }
    }

    message = "Installed " + model->name;
    logger_.info("Model installed", {{"model_id", modelId}});
    return true;
}

} // namespace jarvis::model
