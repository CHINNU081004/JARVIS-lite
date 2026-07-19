#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace jarvis::model {

enum class ModelCategory {
    GeneralAssistant,
    Coding,
    Multimodal,
    SpeechToText,
    TextToSpeech,
    Custom
};

struct ModelInfo {
    std::string id;
    std::string name;
    std::string family;
    ModelCategory category;
    std::string quantization;
    std::uint64_t sizeBytes;
    std::uint64_t recommendedRamBytes;
    unsigned int contextLength;
    std::string downloadUrl;
    std::string sha256;
    std::string description;
};

std::string toString(ModelCategory category);

class ModelCatalog {
public:
    static std::vector<ModelInfo> builtInModels();
    static std::optional<ModelInfo> findById(const std::string& id);
};

} // namespace jarvis::model
