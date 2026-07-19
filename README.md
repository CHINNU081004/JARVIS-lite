# JARVIS Lite

Offline-first C++ terminal assistant foundation with model management, local agent tools, voice module boundaries, and Linux/Windows release packaging.

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
- Curated model catalog covering Gemma 4 multimodal models and coding-specialized GGUF models.
- Custom GGUF download command for user-provided model URLs.
- Model manager commands for listing, downloading, verifying, selecting, and removing models.
- Resume-capable downloads when built with libcurl.
- SHA-256 integrity verification when a checksum is configured.
- Hardware detection for OS, CPU architecture, CPU cores, RAM, and storage.
- Hardware-aware model availability and recommendation.
- Structured JSON logging.
- Configuration for active model, model directory, download location, thread count, context length, and log level.
- Basic local agent capabilities for files, folders, and confirmed shell command execution.
- Voice command surface for speech-to-text and text-to-speech runtime integration.

## Supported Platforms

The codebase is designed for:

- Windows x64 and ARM64
- Linux x64 and ARM64

Current release packaging builds Linux x64 and Windows x64 packages.

## Release Packages

GitHub Releases provide ready-to-test builds:

- `jarvis-lite-linux-x64.tar.gz`: Linux package with the `jarvis-lite` binary, license, README, and installer script.
- `jarvis-lite-windows-x64.exe`: Windows terminal executable for PowerShell or Command Prompt.

JARVIS Lite is a terminal application. On Linux, run the package installer or execute the binary directly:

```sh
tar -xzf jarvis-lite-linux-x64.tar.gz
./install.sh
jarvis-lite --version
```

From a source checkout, the local build can also be opened directly:

```sh
./build/jarvis-lite
```

On Windows, run the downloaded executable from PowerShell:

```powershell
.\jarvis-lite-windows-x64.exe --version
```

## Supported Model Families

- Gemma 4 E2B, E4B, 12B, 26B A4B, and 31B Q4_0 GGUF models
- Qwen2.5 Coder GGUF models
- DeepSeek Coder GGUF models
- Custom direct GGUF URLs provided by the user

Models are not bundled with the executable.

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

Install the command into your user PATH on Linux or macOS:

```sh
cmake --install build --prefix "$HOME/.local"
export PATH="$HOME/.local/bin:$PATH"
jarvis-lite --version
```

On Linux and macOS the command is lowercase: `jarvis-lite`. Running `JARVIS-lite` will fail on case-sensitive shells.

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
/models download gemma-4-e2b-it-q4
/models download-url my-model https://huggingface.co/user/repo/resolve/main/model.gguf
/models select gemma-4-e2b-it-q4
/voice status
/voice stt speech.wav
/voice tts hello
/agent ls .
/agent read README.md
/run cmake --version
/exit
```

See [docs/MODELS.md](docs/MODELS.md) for model download notes.

Terminal commands require explicit confirmation.

## Current Inference Status

Version 1.0 defines the inference, model, and voice boundaries, but this scaffold does not bundle or link a GGUF, STT, or TTS runtime. A production release should connect `LocalInferenceEngine` to the chosen local inference backend and `VoiceEngine` to Whisper/Piper-style backends while keeping the CLI and core architecture unchanged.

## Roadmap

See [ROADMAP.md](ROADMAP.md).

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## Security

See [SECURITY.md](SECURITY.md).

## License

JARVIS Lite is released under the MIT License. See [LICENSE](LICENSE).
