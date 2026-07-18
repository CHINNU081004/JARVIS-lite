#include "jarvis/core/Application.hpp"

#include "jarvis/agent/FileAgent.hpp"
#include "jarvis/cli/CommandLoop.hpp"
#include "jarvis/config/Config.hpp"
#include "jarvis/core/Version.hpp"
#include "jarvis/inference/InferenceEngine.hpp"
#include "jarvis/logging/Logger.hpp"
#include "jarvis/model/ModelManager.hpp"
#include "jarvis/platform/SystemInfo.hpp"

#include <iostream>
#include <stdexcept>

namespace jarvis::core {
namespace {

void printUsage(std::ostream& output) {
    output << AppName << " " << AppVersion << '\n'
           << "Usage: jarvis-lite [--help] [--version] [--hardware] [--models]\n"
           << "Run without arguments to start the interactive CLI.\n";
}

} // namespace

int Application::run(const std::vector<std::string>& arguments) {
    try {
        if (arguments.size() > 1) {
            const auto& arg = arguments[1];
            if (arg == "--help" || arg == "-h") {
                printUsage(std::cout);
                return 0;
            }
            if (arg == "--version" || arg == "-v") {
                std::cout << AppName << " " << AppVersion << '\n';
                return 0;
            }
        }

        auto configManager = config::ConfigManager(config::ConfigManager::defaultConfigPath());
        const bool loaded = configManager.load();
        if (!loaded) {
            try {
                configManager.save();
            } catch (const std::exception& error) {
                std::cerr << "Warning: unable to write default config: " << error.what() << '\n';
            }
        }

        logging::Logger logger(configManager.config().loggingLevel);
        auto system = platform::detectSystemInfo(configManager.config().modelDirectory);
        model::ModelManager modelManager(configManager.config(), logger);
        inference::LocalInferenceEngine inferenceEngine(modelManager);
        agent::FileAgent fileAgent;

        if (arguments.size() > 1) {
            const auto& arg = arguments[1];
            if (arg == "--hardware") {
                std::cout << "Operating system: " << system.operatingSystem << '\n'
                          << "Architecture: " << system.architecture << '\n'
                          << "CPU cores: " << system.cpuCores << '\n'
                          << "Total RAM: " << platform::formatBytes(system.totalRamBytes) << '\n'
                          << "Available RAM: " << platform::formatBytes(system.availableRamBytes) << '\n'
                          << "Storage free: " << platform::formatBytes(system.storageAvailableBytes) << '\n';
                return 0;
            }
            if (arg == "--models") {
                for (const auto& item : modelManager.availableModels(system)) {
                    std::cout << item.model.id << " | " << item.model.name;
                    if (!item.availableForHardware) {
                        std::cout << " | unavailable: " << item.reason;
                    }
                    std::cout << '\n';
                }
                return 0;
            }
            printUsage(std::cerr);
            return 2;
        }

        cli::CommandLoop loop(configManager, logger, modelManager, inferenceEngine, fileAgent, system, std::cin, std::cout);
        return loop.run();
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << '\n';
        return 1;
    }
}

} // namespace jarvis::core
