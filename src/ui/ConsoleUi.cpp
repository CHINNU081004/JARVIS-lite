#include "jarvis/ui/ConsoleUi.hpp"

#include "jarvis/core/Version.hpp"
#include "jarvis/util/String.hpp"

#include <cstdint>
#include <sstream>
#include <utility>

namespace jarvis::ui {

ConsoleUi::ConsoleUi(config::ConfigManager& configManager,
                     model::ModelManager& modelManager,
                     inference::InferenceEngine& inferenceEngine,
                     voice::VoiceEngine& voiceEngine,
                     platform::SystemInfo system,
                     std::istream& input,
                     std::ostream& output)
    : configManager_(configManager),
      modelManager_(modelManager),
      inferenceEngine_(inferenceEngine),
      voiceEngine_(voiceEngine),
      system_(std::move(system)),
      input_(input),
      output_(output) {}

int ConsoleUi::run() {
    bool running = true;
    while (running) {
        drawHome();
        const auto choice = readLine("Select");
        if (choice == "1") {
            chat();
        } else if (choice == "2") {
            showModels();
        } else if (choice == "3") {
            downloadModel();
        } else if (choice == "4") {
            selectModel();
        } else if (choice == "5") {
            voiceConversation();
        } else if (choice == "6") {
            showVoiceStatus();
        } else if (choice == "7") {
            showHardware();
        } else if (choice == "0" || util::toLower(choice) == "q") {
            running = false;
        } else {
            output_ << "Unknown option.\n";
        }
    }
    return 0;
}

void ConsoleUi::drawHome() const {
    output_ << "\n=== " << core::AppName << " " << core::AppVersion << " ===\n"
            << "Active model: " << configManager_.config().activeModel << "\n\n"
            << "1. Chat\n"
            << "2. List models\n"
            << "3. Download model\n"
            << "4. Select model\n"
            << "5. Voice conversation from audio file\n"
            << "6. Voice status\n"
            << "7. Hardware\n"
            << "0. Exit\n";
}

void ConsoleUi::showHardware() const {
    output_ << "Operating system: " << system_.operatingSystem << '\n'
            << "Architecture:     " << system_.architecture << '\n'
            << "CPU cores:        " << system_.cpuCores << '\n'
            << "Total RAM:        " << platform::formatBytes(system_.totalRamBytes) << '\n'
            << "Available RAM:    " << platform::formatBytes(system_.availableRamBytes) << '\n'
            << "Storage free:     " << platform::formatBytes(system_.storageAvailableBytes) << '\n';
}

void ConsoleUi::showModels() const {
    for (const auto& item : modelManager_.availableModels(system_)) {
        output_ << item.model.id << " | " << item.model.name
                << " | " << model::toString(item.model.category)
                << " | size " << platform::formatBytes(item.model.sizeBytes)
                << " | RAM " << platform::formatBytes(item.model.recommendedRamBytes)
                << " | " << item.model.description;
        if (!item.availableForHardware) {
            output_ << " | unavailable: " << item.reason;
        }
        output_ << '\n';
    }
}

void ConsoleUi::downloadModel() {
    const auto id = readLine("Model ID");
    if (id.empty()) {
        return;
    }

    std::string message;
    const bool ok = modelManager_.downloadModel(id, message, [&](std::uint64_t downloaded, std::uint64_t total) {
        if (total == 0) {
            return;
        }
        output_ << "\rDownloading: " << static_cast<int>((downloaded * 100ULL) / total) << '%' << std::flush;
        if (downloaded >= total) {
            output_ << '\n';
        }
    });
    output_ << (ok ? "OK: " : "Error: ") << message << '\n';
}

void ConsoleUi::selectModel() {
    const auto id = readLine("Model ID");
    if (id.empty()) {
        return;
    }

    std::string message;
    const bool ok = modelManager_.selectModel(id, message);
    if (ok) {
        configManager_.save();
    }
    output_ << (ok ? "OK: " : "Error: ") << message << '\n';
}

void ConsoleUi::chat() {
    const auto prompt = readLine("You");
    if (!prompt.empty()) {
        generateResponse(prompt, false);
    }
}

void ConsoleUi::voiceConversation() {
    const auto audioPath = readLine("Audio file");
    if (audioPath.empty()) {
        return;
    }

    std::string transcript;
    const auto result = voiceEngine_.transcribe(audioPath, transcript);
    if (!result.success) {
        output_ << "Error: " << result.message << '\n';
        return;
    }

    output_ << "You said: " << transcript << '\n';
    generateResponse(transcript, true);
}

void ConsoleUi::showVoiceStatus() const {
    const auto status = voiceEngine_.status();
    output_ << "Speech-to-text: " << (status.speechToTextAvailable ? "available" : "unavailable")
            << " | " << status.speechToTextBackend << '\n'
            << "Text-to-speech: " << (status.textToSpeechAvailable ? "available" : "unavailable")
            << " | " << status.textToSpeechBackend << '\n';
}

std::string ConsoleUi::readLine(const std::string& prompt) {
    output_ << prompt << ": " << std::flush;
    std::string value;
    std::getline(input_, value);
    return util::trim(value);
}

std::string ConsoleUi::generateResponse(const std::string& prompt, bool speakResponse) {
    history_.push_back({"user", prompt});
    output_ << "Assistant: " << std::flush;

    std::ostringstream response;
    inferenceEngine_.streamResponse(prompt, history_, [&](const std::string& token) {
        output_ << token << std::flush;
        response << token;
    });

    auto finalResponse = util::trim(response.str());
    history_.push_back({"assistant", finalResponse});
    if (speakResponse) {
        const auto result = voiceEngine_.speak(finalResponse);
        if (!result.success) {
            output_ << "Voice output unavailable: " << result.message << '\n';
        }
    }
    return finalResponse;
}

} // namespace jarvis::ui
