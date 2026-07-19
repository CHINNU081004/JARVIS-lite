#include "jarvis/voice/VoiceEngine.hpp"

#include "jarvis/util/String.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>

namespace jarvis::voice {
namespace {

#if defined(_WIN32)
FILE* openPipe(const std::string& command, const char* mode) {
    return _popen(command.c_str(), mode);
}

int closePipe(FILE* pipe) {
    return _pclose(pipe);
}
#else
FILE* openPipe(const std::string& command, const char* mode) {
    return popen(command.c_str(), mode);
}

int closePipe(FILE* pipe) {
    return pclose(pipe);
}
#endif

std::string envValue(const char* name) {
    const char* value = std::getenv(name);
    if (value == nullptr || *value == '\0') {
        return {};
    }
    return value;
}

std::string shellQuote(const std::string& value) {
#if defined(_WIN32)
    std::string quoted = "\"";
    for (const char character : value) {
        if (character == '"') {
            quoted += "\\\"";
        } else {
            quoted += character;
        }
    }
    quoted += "\"";
    return quoted;
#else
    std::string quoted = "'";
    for (const char character : value) {
        if (character == '\'') {
            quoted += "'\\''";
        } else {
            quoted += character;
        }
    }
    quoted += "'";
    return quoted;
#endif
}

#if defined(_WIN32)
std::string powershellSingleQuote(const std::string& value) {
    std::string quoted = "'";
    for (const char character : value) {
        if (character == '\'') {
            quoted += "''";
        } else {
            quoted += character;
        }
    }
    quoted += "'";
    return quoted;
}
#endif

void replaceAll(std::string& value, const std::string& from, const std::string& to) {
    std::size_t position = 0;
    while ((position = value.find(from, position)) != std::string::npos) {
        value.replace(position, from.size(), to);
        position += to.size();
    }
}

std::string renderCommand(std::string command, const std::string& placeholder, const std::string& value) {
    replaceAll(command, placeholder, shellQuote(value));
    return command;
}

bool commandSucceeds(const std::string& command) {
    return std::system(command.c_str()) == 0;
}

std::string captureCommand(const std::string& command, int& exitCode) {
    std::array<char, 4096> buffer {};
    std::ostringstream output;
    FILE* pipe = openPipe(command, "r");
    if (pipe == nullptr) {
        exitCode = -1;
        return {};
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        output << buffer.data();
    }

    exitCode = closePipe(pipe);
    return output.str();
}

std::string defaultTextToSpeechCommand(const std::string& text) {
#if defined(_WIN32)
    return "powershell -NoProfile -Command \"Add-Type -AssemblyName System.Speech; "
           "$s = New-Object System.Speech.Synthesis.SpeechSynthesizer; "
           "$s.Speak(" + powershellSingleQuote(text) + ")\"";
#else
    const auto quoted = shellQuote(text);
    if (commandSucceeds("command -v espeak-ng >/dev/null 2>&1")) {
        return "espeak-ng " + quoted;
    }
    if (commandSucceeds("command -v espeak >/dev/null 2>&1")) {
        return "espeak " + quoted;
    }
    if (commandSucceeds("command -v spd-say >/dev/null 2>&1")) {
        return "spd-say " + quoted;
    }
    return {};
#endif
}

} // namespace

VoiceStatus VoiceEngine::status() const {
    const auto sttCommand = envValue("JARVIS_STT_COMMAND");
    const auto ttsCommand = envValue("JARVIS_TTS_COMMAND");

    const bool defaultTtsAvailable =
#if defined(_WIN32)
        true;
#else
        commandSucceeds("command -v espeak-ng >/dev/null 2>&1")
        || commandSucceeds("command -v espeak >/dev/null 2>&1")
        || commandSucceeds("command -v spd-say >/dev/null 2>&1");
#endif

    return {
        .speechToTextAvailable = !sttCommand.empty(),
        .textToSpeechAvailable = !ttsCommand.empty() || defaultTtsAvailable,
        .speechToTextBackend = sttCommand.empty()
            ? "set JARVIS_STT_COMMAND with {audio} placeholder"
            : sttCommand,
        .textToSpeechBackend = !ttsCommand.empty()
            ? ttsCommand
            : (defaultTtsAvailable ? "system speech command" : "set JARVIS_TTS_COMMAND with {text} placeholder")
    };
}

VoiceResult VoiceEngine::transcribe(const std::filesystem::path& audioPath, std::string& transcript) const {
    transcript.clear();
    if (!std::filesystem::exists(audioPath)) {
        return {false, "Audio file does not exist: " + audioPath.string()};
    }

    auto sttCommand = envValue("JARVIS_STT_COMMAND");
    if (sttCommand.empty()) {
        return {false, "Set JARVIS_STT_COMMAND to an STT command that prints a transcript. Use {audio} for the audio file path."};
    }

    const auto command = renderCommand(std::move(sttCommand), "{audio}", audioPath.string());
    int exitCode = 0;
    transcript = util::trim(captureCommand(command, exitCode));
    if (exitCode != 0) {
        return {false, "Speech-to-text command failed."};
    }
    if (transcript.empty()) {
        return {false, "Speech-to-text command returned an empty transcript."};
    }

    return {true, "Transcription complete."};
}

VoiceResult VoiceEngine::speak(const std::string& text) const {
    if (text.empty()) {
        return {false, "Text-to-speech requires non-empty text."};
    }

    auto command = envValue("JARVIS_TTS_COMMAND");
    if (!command.empty()) {
        command = renderCommand(std::move(command), "{text}", text);
    } else {
        command = defaultTextToSpeechCommand(text);
    }

    if (command.empty()) {
        return {false, "No text-to-speech backend found. Install espeak-ng/espeak/spd-say or set JARVIS_TTS_COMMAND with {text}."};
    }

    if (std::system(command.c_str()) != 0) {
        return {false, "Text-to-speech command failed."};
    }

    return {true, "Speech played."};
}

} // namespace jarvis::voice
