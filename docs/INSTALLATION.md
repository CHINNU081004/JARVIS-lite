# Installation Guide

JARVIS Lite currently supports source builds.

## From Source

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build
```

The installed command is:

```sh
jarvis-lite
```

## Models

Models are downloaded after installation. They are not bundled with the executable.

Default model storage locations:

- Linux: `$XDG_DATA_HOME/jarvis-lite/models` or `~/.local/share/jarvis-lite/models`
- macOS: `~/Library/Application Support/JARVIS Lite/models`
- Windows: `%LOCALAPPDATA%\\JARVIS Lite\\models`

Model URLs and checksums must be configured by the distribution before public model downloads are enabled.

