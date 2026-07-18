# JARVIS Lite

JARVIS Lite is an offline-first, cross-platform command line AI assistant written in modern C++23. Version 1.0 focuses on a stable native CLI foundation: local chat flow, model management, hardware detection, structured logging, configuration, and basic file-system agent tools.

This repository intentionally avoids Electron, Chromium, WebView, Node.js, and Python runtime dependencies.

## Goals

- Run locally on the user's computer.
- Work offline after models are downloaded.
- Keep platform-specific code isolated.
- Stay lightweight, auditable, and easy to extend.
- Provide a clean foundation for future native GUI and deeper OS integration.

## Features

- Interactive CLI chat loop.
- Streaming response interface.
- Session history for the current run.
- Help, version, clear, and exit commands.
- Model catalog for Gemma, Qwen Coder, and DeepSeek Coder families.
- Model manager commands for listing, downloading, verifying, selecting, and removing models.
- Resume-capable downloads when built with libcurl.
- SHA-256 integrity verification when a checksum is configured.
- Hardware detection for OS, CPU architecture, CPU cores, RAM, and storage.
- Hardware-aware model availability and recommendation.
- Structured JSON logging.
- Configuration for active model, model directory, download location, thread count, context length, and log level.
- Basic local agent capabilities for files, folders, and confirmed shell command execution.

## Supported Platforms

The codebase is designed for:

- Windows x64 and ARM64
- Linux x64 and ARM64
- macOS Intel and Apple Silicon

The current CI setup is intentionally minimal and should be expanded as public infrastructure is added.

## Supported Model Families

- Gemma for general assistant use
- Qwen Coder for coding assistance
- DeepSeek Coder for coding assistance

Models are not bundled with the executable. Distribution-specific model URLs and checksums should be configured before public binary releases.

## Build From Source

Requirements:

- CMake 3.24 or newer
- A C++23 compiler
- Optional: libcurl development package for HTTP model downloads

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Run:

```sh
./build/jarvis-lite
```

Show hardware:

```sh
./build/jarvis-lite --hardware
```

Use a custom config location:

```sh
JARVIS_LITE_CONFIG=/path/to/config.toml ./build/jarvis-lite
```

List model catalog:

```sh
./build/jarvis-lite --models
```

## CLI Usage

Start the application with no arguments:

```sh
jarvis-lite
```

Useful commands:

```text
/help
/version
/hardware
/models available
/models recommend
/models installed
/models download <id>
/models select <id>
/agent ls .
/agent read README.md
/run cmake --version
/exit
```

Terminal commands require explicit confirmation.

## Current Inference Status

Version 1.0 defines the inference boundary and streaming interface, but this scaffold does not bundle or link a GGUF runtime. A production release should connect `LocalInferenceEngine` to the chosen local inference backend while keeping the CLI and core architecture unchanged.

## Roadmap

See [ROADMAP.md](ROADMAP.md).

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## Security

See [SECURITY.md](SECURITY.md).

## License

JARVIS Lite is released under the MIT License. See [LICENSE](LICENSE).
