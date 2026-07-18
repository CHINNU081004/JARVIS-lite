#include "jarvis/model/ModelCatalog.hpp"

#include <algorithm>

namespace jarvis::model {

std::string toString(ModelCategory category) {
    switch (category) {
    case ModelCategory::GeneralAssistant:
        return "general-assistant";
    case ModelCategory::Coding:
        return "coding";
    }
    return "unknown";
}

std::vector<ModelInfo> ModelCatalog::builtInModels() {
    return {
        {
            .id = "gemma-2b-it-q4",
            .name = "Gemma 2B Instruct Q4",
            .family = "Gemma",
            .category = ModelCategory::GeneralAssistant,
            .quantization = "Q4",
            .sizeBytes = 2ULL * 1024ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 6ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 4096,
            .downloadUrl = "",
            .sha256 = "",
            .description = "Default lightweight assistant model target for constrained machines."
        },
        {
            .id = "gemma-7b-it-q4",
            .name = "Gemma 7B Instruct Q4",
            .family = "Gemma",
            .category = ModelCategory::GeneralAssistant,
            .quantization = "Q4",
            .sizeBytes = 5ULL * 1024ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 12ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 8192,
            .downloadUrl = "",
            .sha256 = "",
            .description = "Larger general assistant target for systems with more memory."
        },
        {
            .id = "qwen-coder-1.5b-q4",
            .name = "Qwen Coder 1.5B Q4",
            .family = "Qwen Coder",
            .category = ModelCategory::Coding,
            .quantization = "Q4",
            .sizeBytes = 1300ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 5ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 8192,
            .downloadUrl = "",
            .sha256 = "",
            .description = "Small coding model target for basic offline programming assistance."
        },
        {
            .id = "qwen-coder-7b-q4",
            .name = "Qwen Coder 7B Q4",
            .family = "Qwen Coder",
            .category = ModelCategory::Coding,
            .quantization = "Q4",
            .sizeBytes = 5ULL * 1024ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 12ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 8192,
            .downloadUrl = "",
            .sha256 = "",
            .description = "Balanced offline coding model target."
        },
        {
            .id = "deepseek-coder-1.3b-q4",
            .name = "DeepSeek Coder 1.3B Q4",
            .family = "DeepSeek Coder",
            .category = ModelCategory::Coding,
            .quantization = "Q4",
            .sizeBytes = 1200ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 5ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 4096,
            .downloadUrl = "",
            .sha256 = "",
            .description = "Small DeepSeek coding model target for constrained machines."
        },
        {
            .id = "deepseek-coder-6.7b-q4",
            .name = "DeepSeek Coder 6.7B Q4",
            .family = "DeepSeek Coder",
            .category = ModelCategory::Coding,
            .quantization = "Q4",
            .sizeBytes = 4800ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 12ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 8192,
            .downloadUrl = "",
            .sha256 = "",
            .description = "Larger DeepSeek coding model target."
        }
    };
}

std::optional<ModelInfo> ModelCatalog::findById(const std::string& id) {
    const auto models = builtInModels();
    const auto found = std::find_if(models.begin(), models.end(), [&](const ModelInfo& model) {
        return model.id == id;
    });
    if (found == models.end()) {
        return std::nullopt;
    }
    return *found;
}

} // namespace jarvis::model

