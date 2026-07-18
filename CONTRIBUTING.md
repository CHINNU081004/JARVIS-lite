# Contributing

Thanks for helping improve JARVIS Lite.

## Development Requirements

- CMake 3.24 or newer
- C++23 compiler
- Optional libcurl development package for model downloads

## Workflow

1. Keep changes focused.
2. Build locally.
3. Run tests.
4. Update documentation for user-visible behavior.
5. Avoid committing secrets, tokens, model files, or personal data.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

## Style

- Prefer simple, portable C++23.
- Keep platform-specific code under `platform`.
- Keep inference runtime details behind `InferenceEngine`.
- Keep model download and verification logic under `model` and `network`.
- Use structured logging for operational events.

## Pull Requests

Pull requests should include:

- Summary of the change
- Tests run
- Documentation updates, if applicable
- Any platform limitations

