# Build Guide

## Requirements

- CMake 3.24 or newer
- C++23 compiler
- Optional libcurl development files for HTTP model downloads

## Configure

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

Disable libcurl detection:

```sh
cmake -S . -B build -DJARVIS_ENABLE_CURL=OFF
```

Disable tests:

```sh
cmake -S . -B build -DJARVIS_BUILD_TESTS=OFF
```

## Build

```sh
cmake --build build
```

## Test

```sh
ctest --test-dir build --output-on-failure
```

## Install

```sh
cmake --install build
```

