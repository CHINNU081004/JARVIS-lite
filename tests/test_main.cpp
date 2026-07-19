#include "jarvis/agent/FileAgent.hpp"
#include "jarvis/config/Config.hpp"
#include "jarvis/model/ModelManager.hpp"
#include "jarvis/platform/SystemInfo.hpp"
#include "jarvis/util/Sha256.hpp"
#include "jarvis/util/String.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace {

void testStringUtilities() {
    assert(jarvis::util::trim("  hello  ") == "hello");
    assert(jarvis::util::toLower("HeLLo") == "hello");
    assert(jarvis::util::startsWith("jarvis", "jar"));
}

void testSha256() {
    assert(jarvis::util::sha256Hex("abc") == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

void testConfigRoundTrip() {
    jarvis::config::Config config;
    config.activeModel = "gemma-4-e2b-it-q4";
    config.downloadLocation = "/tmp/jarvis-downloads";
    config.modelDirectory = "/tmp/jarvis-models";
    config.threadCount = 4;
    config.contextLength = 2048;
    config.loggingLevel = jarvis::logging::LogLevel::Debug;

    const auto serialized = jarvis::config::serializeConfig(config);
    const auto parsed = jarvis::config::parseConfig(serialized);
    assert(parsed.activeModel == config.activeModel);
    assert(parsed.downloadLocation == config.downloadLocation);
    assert(parsed.modelDirectory == config.modelDirectory);
    assert(parsed.threadCount == config.threadCount);
    assert(parsed.contextLength == config.contextLength);
    assert(parsed.loggingLevel == config.loggingLevel);
}

void testFileAgent() {
    const auto root = std::filesystem::temp_directory_path() / "jarvis-lite-agent-test";
    std::filesystem::remove_all(root);

    jarvis::agent::FileAgent agent;
    auto result = agent.createDirectory(root);
    assert(result.success);

    const auto file = root / "note.txt";
    result = agent.writeFile(file, "hello");
    assert(result.success);

    std::string content;
    result = agent.readFile(file, content);
    assert(result.success);
    assert(content == "hello");

    std::vector<jarvis::agent::DirectoryEntry> entries;
    result = agent.listDirectory(root, entries);
    assert(result.success);
    assert(!entries.empty());

    std::filesystem::remove_all(root);
}

void testModelRecommendation() {
    jarvis::config::Config config;
    config.modelDirectory = std::filesystem::temp_directory_path() / "jarvis-lite-model-test";
    std::ostringstream logSink;
    jarvis::logging::Logger logger(jarvis::logging::LogLevel::Error, &logSink);
    jarvis::model::ModelManager manager(config, logger);

    jarvis::platform::SystemInfo system;
    system.totalRamBytes = 8ULL * 1024ULL * 1024ULL * 1024ULL;
    system.storageAvailableBytes = 20ULL * 1024ULL * 1024ULL * 1024ULL;
    const auto recommendation = manager.recommendedModel(system);
    assert(recommendation.has_value());
    assert(recommendation->family == "Gemma");
}

} // namespace

int main() {
    testStringUtilities();
    testSha256();
    testConfigRoundTrip();
    testFileAgent();
    testModelRecommendation();
    std::cout << "All tests passed\n";
    return 0;
}
