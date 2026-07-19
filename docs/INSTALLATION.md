# Installation Guide

JARVIS Lite is a terminal application. The command name is lowercase:

```sh
jarvis-lite
```

On Linux, `JARVIS-lite` is a different command and will fail with `command not found`.

## From GitHub Releases

Download the package for your operating system from the GitHub Releases page. Current packaging focus is Linux and Windows.

### Linux

```sh
tar -xzf jarvis-lite-linux-x64.tar.gz
./install.sh
jarvis-lite --version
```

The installer copies the binary to `$HOME/.local/bin` by default. If that directory is not on PATH, add it:

```sh
export PATH="$HOME/.local/bin:$PATH"
```

### Windows

Download `jarvis-lite-windows-x64.exe` and run it from PowerShell:

```powershell
.\jarvis-lite-windows-x64.exe --version
```

To run it from any folder, put the file in a directory that is already on your PATH, or add its folder to PATH.

## From Source

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix "$HOME/.local"
```

The installed command is:

```sh
jarvis-lite
```

The basic menu UI starts with:

```sh
jarvis-lite --ui
```

If `jarvis-lite` still says `command not found`, add the install directory to PATH:

```sh
export PATH="$HOME/.local/bin:$PATH"
```

## Models

Models are downloaded after installation. They are not bundled with the executable.

Default model storage locations:

- Linux: `$XDG_DATA_HOME/jarvis-lite/models` or `~/.local/share/jarvis-lite/models`
- Windows: `%LOCALAPPDATA%\\JARVIS Lite\\models`

Use `/models available`, `/models download <id>`, or `/models download-url <id> <direct-gguf-url>` inside the app. See [MODELS.md](MODELS.md) for model IDs and Hugging Face token notes.
