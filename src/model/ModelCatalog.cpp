#include "jarvis/model/ModelCatalog.hpp"

#include <algorithm>

namespace jarvis::model {

std::string toString(ModelCategory category) {
    switch (category) {
    case ModelCategory::GeneralAssistant:
        return "general-assistant";
    case ModelCategory::Coding:
        return "coding";
    case ModelCategory::Multimodal:
        return "multimodal";
    case ModelCategory::SpeechToText:
        return "speech-to-text";
    case ModelCategory::TextToSpeech:
        return "text-to-speech";
    case ModelCategory::Custom:
        return "custom";
    }
    return "unknown";
}

std::vector<ModelInfo> ModelCatalog::builtInModels() {
    return {
        {
            .id = "gemma-4-e2b-it-q4",
            .name = "Gemma 4 E2B Instruct Q4_0",
            .family = "Gemma",
            .category = ModelCategory::Multimodal,
            .quantization = "Q4_0",
            .sizeBytes = 3350ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 6ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 131072,
            .downloadUrl = "https://huggingface.co/google/gemma-4-E2B-it-qat-q4_0-gguf/resolve/main/gemma-4-E2B_q4_0-it.gguf",
            .sha256 = "",
            .description = "Best default multimodal starter for laptops; supports Gemma 4 text, image, and audio-capable workflows when the runtime supports them."
        },
        {
            .id = "gemma-4-e4b-it-q4",
            .name = "Gemma 4 E4B Instruct Q4_0",
            .family = "Gemma",
            .category = ModelCategory::Multimodal,
            .quantization = "Q4_0",
            .sizeBytes = 5150ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 10ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 131072,
            .downloadUrl = "https://huggingface.co/google/gemma-4-E4B-it-qat-q4_0-gguf/resolve/main/gemma-4-E4B_q4_0-it.gguf",
            .sha256 = "",
            .description = "Balanced multimodal Gemma 4 model for newer laptops and desktops."
        },
        {
            .id = "gemma-4-12b-it-q4",
            .name = "Gemma 4 12B Instruct Q4_0",
            .family = "Gemma",
            .category = ModelCategory::Multimodal,
            .quantization = "Q4_0",
            .sizeBytes = 6980ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 16ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 262144,
            .downloadUrl = "https://huggingface.co/google/gemma-4-12B-it-qat-q4_0-gguf/resolve/main/gemma-4-12b-it-qat-q4_0.gguf",
            .sha256 = "",
            .description = "Higher-quality multimodal Gemma 4 unified model for capable desktops with more RAM."
        },
        {
            .id = "gemma-4-26b-a4b-it-q4",
            .name = "Gemma 4 26B A4B Instruct Q4_0",
            .family = "Gemma",
            .category = ModelCategory::Multimodal,
            .quantization = "Q4_0",
            .sizeBytes = 14400ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 24ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 262144,
            .downloadUrl = "https://huggingface.co/google/gemma-4-26B-A4B-it-qat-q4_0-gguf/resolve/main/gemma-4-26B_q4_0-it.gguf",
            .sha256 = "",
            .description = "Best high-end Gemma 4 mixture-of-experts option for strong reasoning with faster active-parameter inference."
        },
        {
            .id = "gemma-4-31b-it-q4",
            .name = "Gemma 4 31B Instruct Q4_0",
            .family = "Gemma",
            .category = ModelCategory::Multimodal,
            .quantization = "Q4_0",
            .sizeBytes = 17700ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 28ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 262144,
            .downloadUrl = "https://huggingface.co/google/gemma-4-31B-it-qat-q4_0-gguf/resolve/main/gemma-4-31B_q4_0-it.gguf",
            .sha256 = "",
            .description = "Largest built-in Gemma 4 dense model target for high-memory workstations."
        },
        {
            .id = "qwen2.5-coder-7b-instruct-q4-k-m",
            .name = "Qwen2.5 Coder 7B Instruct Q4_K_M",
            .family = "Qwen Coder",
            .category = ModelCategory::Coding,
            .quantization = "Q4_K_M",
            .sizeBytes = 4680ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 10ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 32768,
            .downloadUrl = "https://huggingface.co/bartowski/Qwen2.5-Coder-7B-Instruct-GGUF/resolve/main/Qwen2.5-Coder-7B-Instruct-Q4_K_M.gguf",
            .sha256 = "",
            .description = "Best compact coding model target for code generation, bug fixing, and local developer workflows."
        },
        {
            .id = "deepseek-coder-v2-lite-base-iq4-xs",
            .name = "DeepSeek Coder V2 Lite Base IQ4_XS",
            .family = "DeepSeek Coder",
            .category = ModelCategory::Coding,
            .quantization = "IQ4_XS",
            .sizeBytes = 8650ULL * 1024ULL * 1024ULL,
            .recommendedRamBytes = 16ULL * 1024ULL * 1024ULL * 1024ULL,
            .contextLength = 16384,
            .downloadUrl = "https://huggingface.co/mradermacher/DeepSeek-Coder-V2-Lite-Base-GGUF/resolve/main/DeepSeek-Coder-V2-Lite-Base.IQ4_XS.gguf",
            .sha256 = "",
            .description = "Larger coding-specialized option for machines with more RAM."
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
