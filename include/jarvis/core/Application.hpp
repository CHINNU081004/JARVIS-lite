#pragma once

#include <vector>
#include <string>

namespace jarvis::core {

class Application {
public:
    int run(const std::vector<std::string>& arguments);
};

} // namespace jarvis::core

