#pragma once

#include <filesystem>
#include <string>

namespace jarvis::voice {

struct VoiceStatus {
    bool speechToTextAvailable = false;
    bool textToSpeechAvailable = false;
    std::string speechToTextBackend;
    std::string textToSpeechBackend;
};

struct VoiceResult {
    bool success = false;
    std::string message;
};

class VoiceEngine {
public:
    VoiceStatus status() const;
    VoiceResult transcribe(const std::filesystem::path& audioPath, std::string& transcript) const;
    VoiceResult speak(const std::string& text) const;
};

} // namespace jarvis::voice
