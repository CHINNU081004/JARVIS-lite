cmake_minimum_required(VERSION 3.24)

project(JARVISLite
    VERSION 1.0.0
    DESCRIPTION "Offline-first cross-platform CLI AI assistant"
    LANGUAGES CXX
)

option(JARVIS_ENABLE_CURL "Enable model downloads through libcurl when available" ON)
option(JARVIS_BUILD_TESTS "Build JARVIS Lite tests" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Threads REQUIRED)

if(JARVIS_ENABLE_CURL)
    find_package(CURL QUIET)
endif()

set(JARVIS_PUBLIC_HEADERS
    include/jarvis/agent/FileAgent.hpp
    include/jarvis/cli/CommandLoop.hpp
    include/jarvis/config/Config.hpp
    include/jarvis/core/Application.hpp
    include/jarvis/core/Version.hpp
    include/jarvis/inference/InferenceEngine.hpp
    include/jarvis/logging/Logger.hpp
    include/jarvis/model/ModelCatalog.hpp
    include/jarvis/model/ModelManager.hpp
    include/jarvis/network/Downloader.hpp
    include/jarvis/platform/SystemInfo.hpp
    include/jarvis/util/Sha256.hpp
    include/jarvis/util/String.hpp
    include/jarvis/ui/ConsoleUi.hpp
    include/jarvis/voice/VoiceEngine.hpp
)

set(JARVIS_SOURCES
    src/agent/FileAgent.cpp
    src/cli/CommandLoop.cpp
    src/config/Config.cpp
    src/core/Application.cpp
    src/inference/InferenceEngine.cpp
    src/logging/Logger.cpp
    src/model/ModelCatalog.cpp
    src/model/ModelManager.cpp
    src/network/Downloader.cpp
    src/platform/SystemInfo.cpp
    src/util/Sha256.cpp
    src/util/String.cpp
    src/ui/ConsoleUi.cpp
    src/voice/VoiceEngine.cpp
)

add_library(jarvis_core STATIC ${JARVIS_PUBLIC_HEADERS} ${JARVIS_SOURCES})
target_include_directories(jarvis_core PUBLIC include)
target_link_libraries(jarvis_core PUBLIC Threads::Threads)

if(CURL_FOUND)
    target_link_libraries(jarvis_core PRIVATE CURL::libcurl)
    target_compile_definitions(jarvis_core PRIVATE JARVIS_HAS_CURL=1)
endif()

add_executable(jarvis-lite src/main.cpp)
target_link_libraries(jarvis-lite PRIVATE jarvis_core)

include(GNUInstallDirs)
install(TARGETS jarvis-lite
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(JARVIS_BUILD_TESTS)
    enable_testing()
    add_executable(jarvis_tests tests/test_main.cpp)
    target_link_libraries(jarvis_tests PRIVATE jarvis_core)
    add_test(NAME jarvis_tests COMMAND jarvis_tests)
endif()
