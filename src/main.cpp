#include "jarvis/core/Application.hpp"

#include <string>
#include <vector>

int main(int argc, char** argv) {
    std::vector<std::string> arguments;
    arguments.reserve(static_cast<std::size_t>(argc));
    for (int i = 0; i < argc; ++i) {
        arguments.emplace_back(argv[i]);
    }

    jarvis::core::Application app;
    return app.run(arguments);
}

