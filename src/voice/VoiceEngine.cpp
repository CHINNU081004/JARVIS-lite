#include "jarvis/voice/VoiceEngine.hpp"

#include <filesystem>

namespace jarvis::voice {

VoiceStatus VoiceEngine::status() const {
    return {
        .speechToTextAvailable = false,
        .textToSpeechAvailable = false,
        .speechToTextBackend = "Whisper-compatible backend not linked",
        .textToSpeechBackend = "Piper-compatible backend not linked"
    };
}

VoiceResult VoiceEngine::transcribe(const std::filesystem::path& audioPath, std::string& transcript) const {
    transcript.clear();
    if (!std::filesystem::exists(audioPath)) {
        return {false, "Audio file does not exist: " + audioPath.string()};
    }

    return {false, "Speech-to-text module is present, but no STT runtime is linked yet."};
}

VoiceResult VoiceEngine::speak(const std::string& text) const {
    if (text.empty()) {
        return {false, "Text-to-speech requires non-empty text."};
    }

    return {false, "Text-to-speech module is present, but no TTS runtime is linked yet."};
}

} // namespace jarvis::voice
