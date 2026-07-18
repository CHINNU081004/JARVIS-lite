# Installation Guide

JARVIS Lite is a terminal application. The command name is lowercase:

```sh
jarvis-lite
```

On Linux and macOS, `JARVIS-lite` is a different command and will fail with `command not found`.

## From GitHub Releases

Download the package for your operating system from the GitHub Releases page.

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

### macOS

Open `jarvis-lite-macos.dmg`, then run the included installer from Terminal:

```sh
./install.sh
jarvis-lite --version
```

If macOS blocks the downloaded binary, allow it in System Settings > Privacy & Security.

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

If `jarvis-lite` still says `command not found`, add the install directory to PATH:

```sh
export PATH="$HOME/.local/bin:$PATH"
```

## Models

Models are downloaded after installation. They are not bundled with the executable.

Default model storage locations:

- Linux: `$XDG_DATA_HOME/jarvis-lite/models` or `~/.local/share/jarvis-lite/models`
- macOS: `~/Library/Application Support/JARVIS Lite/models`
- Windows: `%LOCALAPPDATA%\\JARVIS Lite\\models`

Model URLs and checksums must be configured by the distribution before public model downloads are enabled.
