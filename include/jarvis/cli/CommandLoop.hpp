#pragma once

#include "jarvis/agent/FileAgent.hpp"
#include "jarvis/config/Config.hpp"
#include "jarvis/inference/InferenceEngine.hpp"
#include "jarvis/logging/Logger.hpp"
#include "jarvis/model/ModelManager.hpp"
#include "jarvis/platform/SystemInfo.hpp"
#include "jarvis/voice/VoiceEngine.hpp"

#include <iosfwd>
#include <vector>

namespace jarvis::cli {

class CommandLoop {
public:
    CommandLoop(config::ConfigManager& configManager,
                logging::Logger& logger,
                model::ModelManager& modelManager,
                inference::InferenceEngine& inferenceEngine,
                agent::FileAgent& fileAgent,
                voice::VoiceEngine& voiceEngine,
                platform::SystemInfo system,
                std::istream& input,
                std::ostream& output);

    int run();

private:
    bool handleCommand(const std::string& line);
    void printHelp() const;
    void printVersion() const;
    void printHardware() const;
    void printConfig() const;
    void printHistory() const;
    void handleModels(const std::vector<std::string>& parts);
    void handleVoice(const std::string& line, const std::vector<std::string>& parts);
    void handleAgent(const std::string& line, const std::vector<std::string>& parts);
    void handleRun(const std::string& line);
    void chat(const std::string& prompt);

    config::ConfigManager& configManager_;
    logging::Logger& logger_;
    model::ModelManager& modelManager_;
    inference::InferenceEngine& inferenceEngine_;
    agent::FileAgent& fileAgent_;
    voice::VoiceEngine& voiceEngine_;
    platform::SystemInfo system_;
    std::istream& input_;
    std::ostream& output_;
    std::vector<inference::ChatMessage> history_;
    bool running_ = true;
};

} // namespace jarvis::cli
