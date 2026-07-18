#include "jarvis/inference/InferenceEngine.hpp"

#include "jarvis/util/String.hpp"

#include <chrono>
#include <sstream>
#include <thread>

namespace jarvis::inference {
namespace {

void streamWords(const std::string& response, const TokenCallback& onToken) {
    std::istringstream input(response);
    std::string word;
    bool first = true;
    while (input >> word) {
        if (!first) {
            onToken(" ");
        }
        onToken(word);
        first = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }
    onToken("\n");
}

} // namespace

LocalInferenceEngine::LocalInferenceEngine(model::ModelManager& modelManager)
    : modelManager_(modelManager) {}

void LocalInferenceEngine::streamResponse(const std::string& prompt,
                                          const std::vector<ChatMessage>& history,
                                          const TokenCallback& onToken) {
    (void)history;
    const auto normalized = util::toLower(util::trim(prompt));
    const auto active = modelManager_.activeModel();

    if (!active) {
        streamWords("No active model is configured. Use /models available and /models select <id> after installing a model.", onToken);
        return;
    }

    if (!modelManager_.isInstalled(active->id)) {
        streamWords("The active model is " + active->name + ", but it is not installed yet. Use /models download "
            + active->id + " once model URLs are configured for your distribution.", onToken);
        return;
    }

    if (normalized == "hi" || normalized == "hello" || normalized == "hey") {
        streamWords("Hello. JARVIS Lite is running locally with " + active->name + " selected.", onToken);
        return;
    }

    streamWords("Local model execution is isolated behind the inference engine interface. Model file "
        + active->id + " is installed and selected, but this build does not link a GGUF runtime yet.", onToken);
}

} // namespace jarvis::inference

