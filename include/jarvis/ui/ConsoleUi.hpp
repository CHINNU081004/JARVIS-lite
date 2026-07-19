#pragma once

#include "jarvis/config/Config.hpp"
#include "jarvis/inference/InferenceEngine.hpp"
#include "jarvis/model/ModelManager.hpp"
#include "jarvis/platform/SystemInfo.hpp"
#include "jarvis/voice/VoiceEngine.hpp"

#include <iosfwd>
#include <string>
#include <vector>

namespace jarvis::ui {

class ConsoleUi {
public:
    ConsoleUi(config::ConfigManager& configManager,
              model::ModelManager& modelManager,
              inference::InferenceEngine& inferenceEngine,
              voice::VoiceEngine& voiceEngine,
              platform::SystemInfo system,
              std::istream& input,
              std::ostream& output);

    int run();

private:
    void drawHome() const;
    void showHardware() const;
    void showModels() const;
    void downloadModel();
    void selectModel();
    void chat();
    void voiceConversation();
    void showVoiceStatus() const;
    std::string readLine(const std::string& prompt);
    std::string generateResponse(const std::string& prompt, bool speakResponse);

    config::ConfigManager& configManager_;
    model::ModelManager& modelManager_;
    inference::InferenceEngine& inferenceEngine_;
    voice::VoiceEngine& voiceEngine_;
    platform::SystemInfo system_;
    std::istream& input_;
    std::ostream& output_;
    std::vector<inference::ChatMessage> history_;
};

} // namespace jarvis::ui
