# Developer Guide

## Architecture

JARVIS Lite is split into small modules:

- `core`: application startup and version metadata
- `cli`: command loop and user interaction
- `inference`: streaming model runtime boundary
- `model`: model catalog and installed model management
- `network`: resumable downloader abstraction
- `agent`: local file-system and terminal tools
- `config`: persistent settings
- `logging`: structured JSON logging
- `platform`: hardware and operating system detection
- `util`: shared utilities

## Inference Runtime

The CLI depends only on `InferenceEngine`. A production GGUF runtime should be added behind this interface. The runtime should not leak implementation details into the CLI or model manager.

## Model Downloads

`DefaultDownloader` supports local files in all builds. HTTP downloads are enabled when libcurl is found at configure time.

Model integrity is verified with SHA-256 when `ModelInfo::sha256` is configured.

## Platform Code

Keep platform-specific APIs in `src/platform`. Other modules should consume `SystemInfo` rather than calling OS APIs directly.

## Tests

Tests are in `tests/test_main.cpp` and intentionally use no external framework.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

