#pragma once

#include "jarvis/config/Config.hpp"
#include "jarvis/model/ModelManager.hpp"

#include <functional>
#include <string>
#include <vector>

namespace jarvis::inference {

struct ChatMessage {
    std::string role;
    std::string content;
};

using TokenCallback = std::function<void(const std::string& token)>;

class InferenceEngine {
public:
    virtual ~InferenceEngine() = default;
    virtual void streamResponse(const std::string& prompt,
                                const std::vector<ChatMessage>& history,
                                const TokenCallback& onToken) = 0;
};

class LocalInferenceEngine final : public InferenceEngine {
public:
    explicit LocalInferenceEngine(model::ModelManager& modelManager);

    void streamResponse(const std::string& prompt,
                        const std::vector<ChatMessage>& history,
                        const TokenCallback& onToken) override;

private:
    model::ModelManager& modelManager_;
};

} // namespace jarvis::inference

