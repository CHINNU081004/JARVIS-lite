#include "jarvis/cli/CommandLoop.hpp"

#include "jarvis/core/Version.hpp"
#include "jarvis/platform/SystemInfo.hpp"
#include "jarvis/util/String.hpp"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace jarvis::cli {
namespace {

std::string commandPayload(const std::string& line, const std::string& command) {
    if (line.size() <= command.size()) {
        return {};
    }
    return util::trim(line.substr(command.size()));
}

} // namespace

CommandLoop::CommandLoop(config::ConfigManager& configManager,
                         logging::Logger& logger,
                         model::ModelManager& modelManager,
                         inference::InferenceEngine& inferenceEngine,
                         agent::FileAgent& fileAgent,
                         voice::VoiceEngine& voiceEngine,
                         platform::SystemInfo system,
                         std::istream& input,
                         std::ostream& output)
    : configManager_(configManager),
      logger_(logger),
      modelManager_(modelManager),
      inferenceEngine_(inferenceEngine),
      fileAgent_(fileAgent),
      voiceEngine_(voiceEngine),
      system_(std::move(system)),
      input_(input),
      output_(output) {}

int CommandLoop::run() {
    output_ << core::AppName << " " << core::AppVersion << '\n';
    output_ << "Type /help for commands. Type /exit to quit.\n";

    std::string line;
    while (running_) {
        output_ << "\njarvis> " << std::flush;
        if (!std::getline(input_, line)) {
            break;
        }

        line = util::trim(line);
        if (line.empty()) {
            continue;
        }

        if (line.front() == '/') {
            handleCommand(line);
        } else {
            chat(line);
        }
    }

    return 0;
}

bool CommandLoop::handleCommand(const std::string& line) {
    const auto parts = util::splitWhitespace(line);
    if (parts.empty()) {
        return true;
    }

    const auto command = util::toLower(parts.front());
    if (command == "/exit" || command == "/quit") {
        running_ = false;
        return true;
    }
    if (command == "/help") {
        printHelp();
        return true;
    }
    if (command == "/version") {
        printVersion();
        return true;
    }
    if (command == "/clear") {
        output_ << "\033[2J\033[H";
        return true;
    }
    if (command == "/history") {
        printHistory();
        return true;
    }
    if (command == "/hardware") {
        printHardware();
        return true;
    }
    if (command == "/config") {
        printConfig();
        return true;
    }
    if (command == "/models") {
        handleModels(parts);
        return true;
    }
    if (command == "/voice") {
        handleVoice(line, parts);
        return true;
    }
    if (command == "/agent") {
        handleAgent(line, parts);
        return true;
    }
    if (command == "/run") {
        handleRun(line);
        return true;
    }

    output_ << "Unknown command. Type /help.\n";
    return false;
}

void CommandLoop::printHelp() const {
    output_
        << "Commands:\n"
        << "  /help                         Show this help\n"
        << "  /version                      Show version\n"
        << "  /clear                        Clear the terminal\n"
        << "  /history                      Show current session history\n"
        << "  /hardware                     Show detected hardware\n"
        << "  /config                       Show active configuration\n"
        << "  /models available             List supported models and hardware availability\n"
        << "  /models installed             List installed models\n"
        << "  /models recommend             Recommend a model for this machine\n"
        << "  /models download <id>         Download or resume a model download\n"
        << "  /models download-url <id> <url> Download any direct GGUF model URL\n"
        << "  /models verify <id>           Verify an installed model\n"
        << "  /models select <id>           Select active model\n"
        << "  /models remove <id>           Remove installed model\n"
        << "  /voice status                 Show STT/TTS runtime status\n"
        << "  /voice stt <audio-file>       Transcribe an audio file when an STT runtime is linked\n"
        << "  /voice tts <text>             Speak text when a TTS runtime is linked\n"
        << "  /voice chat <audio-file>      Transcribe, answer, and speak when voice backends are configured\n"
        << "  /agent ls [path]              List a directory\n"
        << "  /agent read <path>            Read a file\n"
        << "  /agent write <path> <text>    Write text to a file\n"
        << "  /agent mkdir <path>           Create a folder\n"
        << "  /agent rename <from> <to>     Rename a file or folder\n"
        << "  /agent copy <from> <to>       Copy a file or folder\n"
        << "  /agent move <from> <to>       Move a file or folder\n"
        << "  /run <command>                Execute a terminal command after confirmation\n"
        << "  /exit                         Quit\n";
}

void CommandLoop::printVersion() const {
    output_ << core::AppName << " " << core::AppVersion << '\n'
            << core::AppDescription << '\n';
}

void CommandLoop::printHardware() const {
    output_ << "Operating system: " << system_.operatingSystem << '\n'
            << "Architecture:     " << system_.architecture << '\n'
            << "CPU cores:        " << system_.cpuCores << '\n'
            << "Total RAM:        " << platform::formatBytes(system_.totalRamBytes) << '\n'
            << "Available RAM:    " << platform::formatBytes(system_.availableRamBytes) << '\n'
            << "Storage free:     " << platform::formatBytes(system_.storageAvailableBytes) << '\n';
}

void CommandLoop::printConfig() const {
    const auto& config = configManager_.config();
    output_ << "Config file:       " << configManager_.path() << '\n'
            << "Active model:      " << config.activeModel << '\n'
            << "Model directory:   " << config.modelDirectory << '\n'
            << "Download location: " << config.downloadLocation << '\n'
            << "Thread count:      " << config.threadCount << '\n'
            << "Context length:    " << config.contextLength << '\n'
            << "Logging level:     " << logging::toString(config.loggingLevel) << '\n';
}

void CommandLoop::printHistory() const {
    if (history_.empty()) {
        output_ << "No messages in this session.\n";
        return;
    }
    for (const auto& message : history_) {
        output_ << message.role << ": " << message.content << '\n';
    }
}

void CommandLoop::handleModels(const std::vector<std::string>& parts) {
    if (parts.size() == 1 || parts[1] == "available") {
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
        return;
    }

    const auto action = util::toLower(parts[1]);
    if (action == "installed") {
        const auto installed = modelManager_.installedModels();
        if (installed.empty()) {
            output_ << "No models installed.\n";
            return;
        }
        for (const auto& item : installed) {
            output_ << item.model.id << " | " << item.path;
            if (item.integrityKnown) {
                output_ << " | sha256 " << (item.integrityVerified ? "ok" : "failed");
            } else {
                output_ << " | sha256 unavailable";
            }
            output_ << '\n';
        }
        return;
    }

    if (action == "recommend") {
        const auto recommendation = modelManager_.recommendedModel(system_);
        if (!recommendation) {
            output_ << "No built-in model fits this machine clearly.\n";
            return;
        }
        output_ << "Recommended model: " << recommendation->id << " (" << recommendation->name << ")\n";
        return;
    }

    if (action == "download-url") {
        if (parts.size() < 4) {
            output_ << "Usage: /models download-url <id> <direct-gguf-url>\n";
            return;
        }
        const auto modelId = parts[2];
        const auto url = parts[3];
        std::string message;
        const bool ok = modelManager_.downloadCustomModel(modelId, url, message, [&](std::uint64_t downloaded, std::uint64_t total) {
            if (total == 0) {
                return;
            }
            const auto percent = static_cast<int>((downloaded * 100ULL) / total);
            output_ << "\rDownloading: " << percent << '%' << std::flush;
            if (downloaded >= total) {
                output_ << '\n';
            }
        });
        output_ << (ok ? "OK: " : "Error: ") << message << '\n';
        return;
    }

    if (parts.size() < 3) {
        output_ << "Model command requires a model id. Type /help.\n";
        return;
    }

    const auto modelId = parts[2];
    std::string message;
    bool ok = false;
    if (action == "download") {
        ok = modelManager_.downloadModel(modelId, message, [&](std::uint64_t downloaded, std::uint64_t total) {
            if (total == 0) {
                return;
            }
            const auto percent = static_cast<int>((downloaded * 100ULL) / total);
            output_ << "\rDownloading: " << percent << '%' << std::flush;
            if (downloaded >= total) {
                output_ << '\n';
            }
        });
    } else if (action == "verify") {
        ok = modelManager_.verifyModel(modelId, message);
    } else if (action == "select") {
        ok = modelManager_.selectModel(modelId, message);
        if (ok) {
            configManager_.save();
        }
    } else if (action == "remove") {
        ok = modelManager_.removeModel(modelId, message);
    } else {
        output_ << "Unknown model command. Type /help.\n";
        return;
    }

    output_ << (ok ? "OK: " : "Error: ") << message << '\n';
}

void CommandLoop::handleVoice(const std::string& line, const std::vector<std::string>& parts) {
    if (parts.size() < 2) {
        output_ << "Voice command requires an action. Type /help.\n";
        return;
    }

    const auto action = util::toLower(parts[1]);
    if (action == "status") {
        const auto status = voiceEngine_.status();
        output_ << "Speech-to-text: " << (status.speechToTextAvailable ? "available" : "unavailable")
                << " | " << status.speechToTextBackend << '\n'
                << "Text-to-speech: " << (status.textToSpeechAvailable ? "available" : "unavailable")
                << " | " << status.textToSpeechBackend << '\n';
        return;
    }

    if (action == "stt" && parts.size() >= 3) {
        std::string transcript;
        const auto result = voiceEngine_.transcribe(parts[2], transcript);
        if (!result.success) {
            output_ << "Error: " << result.message << '\n';
            return;
        }
        output_ << transcript << '\n';
        return;
    }

    if (action == "tts") {
        const auto text = commandPayload(line, "/voice tts");
        const auto result = voiceEngine_.speak(text);
        output_ << (result.success ? "OK: " : "Error: ") << result.message << '\n';
        return;
    }

    if (action == "chat" && parts.size() >= 3) {
        std::string transcript;
        const auto result = voiceEngine_.transcribe(parts[2], transcript);
        if (!result.success) {
            output_ << "Error: " << result.message << '\n';
            return;
        }

        output_ << "user voice> " << transcript << '\n';
        const auto response = chat(transcript, true);
        (void)response;
        return;
    }

    output_ << "Invalid voice command. Type /help.\n";
}

void CommandLoop::handleAgent(const std::string& line, const std::vector<std::string>& parts) {
    if (parts.size() < 2) {
        output_ << "Agent command requires an action. Type /help.\n";
        return;
    }

    const auto action = util::toLower(parts[1]);
    if (action == "ls") {
        const std::filesystem::path path = parts.size() >= 3 ? parts[2] : ".";
        std::vector<agent::DirectoryEntry> entries;
        const auto result = fileAgent_.listDirectory(path, entries);
        if (!result.success) {
            output_ << "Error: " << result.message << '\n';
            return;
        }
        for (const auto& entry : entries) {
            output_ << (entry.isDirectory ? "[dir]  " : "[file] ")
                    << std::setw(8) << entry.size << "  "
                    << entry.path.string() << '\n';
        }
        return;
    }

    if (action == "read" && parts.size() >= 3) {
        std::string content;
        const auto result = fileAgent_.readFile(parts[2], content);
        output_ << (result.success ? content : "Error: " + result.message) << '\n';
        return;
    }

    if (action == "write" && parts.size() >= 4) {
        const auto path = parts[2];
        const auto textStart = line.find(path) + path.size();
        const auto content = util::trim(line.substr(textStart));
        const auto result = fileAgent_.writeFile(path, content);
        output_ << (result.success ? "OK: " : "Error: ") << result.message << '\n';
        return;
    }

    if (action == "mkdir" && parts.size() >= 3) {
        const auto result = fileAgent_.createDirectory(parts[2]);
        output_ << (result.success ? "OK: " : "Error: ") << result.message << '\n';
        return;
    }

    if ((action == "rename" || action == "copy" || action == "move") && parts.size() >= 4) {
        agent::AgentResult result;
        if (action == "rename") {
            result = fileAgent_.renamePath(parts[2], parts[3]);
        } else if (action == "copy") {
            result = fileAgent_.copyPath(parts[2], parts[3]);
        } else {
            result = fileAgent_.movePath(parts[2], parts[3]);
        }
        output_ << (result.success ? "OK: " : "Error: ") << result.message << '\n';
        return;
    }

    output_ << "Invalid agent command. Type /help.\n";
}

void CommandLoop::handleRun(const std::string& line) {
    const auto command = commandPayload(line, "/run");
    if (command.empty()) {
        output_ << "Usage: /run <command>\n";
        return;
    }

    output_ << "Execute command? " << command << "\nType yes to continue: " << std::flush;
    std::string confirmation;
    if (!std::getline(input_, confirmation) || util::toLower(util::trim(confirmation)) != "yes") {
        output_ << "Cancelled.\n";
        return;
    }

    int exitCode = 0;
    const auto result = fileAgent_.executeCommand(command, exitCode);
    output_ << (result.success ? "OK: " : "Error: ") << result.message << '\n';
}

std::string CommandLoop::chat(const std::string& prompt, bool speakResponse) {
    history_.push_back({"user", prompt});
    output_ << "assistant> " << std::flush;

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

} // namespace jarvis::cli
