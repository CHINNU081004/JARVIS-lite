# User Guide

## Start

```sh
jarvis-lite
```

Type `/help` to see commands.

## Chat

Enter normal text at the prompt. Responses stream through the inference interface.

## Models

List supported models:

```text
/models available
```

Recommend a model:

```text
/models recommend
```

Download a model:

```text
/models download gemma-2b-it-q4
```

Select a model:

```text
/models select gemma-2b-it-q4
```

## Local Agent Tools

List files:

```text
/agent ls .
```

Read a file:

```text
/agent read README.md
```

Write a file:

```text
/agent write notes/example.txt hello from JARVIS Lite
```

Run a command with confirmation:

```text
/run cmake --version
```

## Configuration

Show configuration:

```text
/config
```

The configuration file is created on first run.

Set a custom configuration path:

```sh
JARVIS_LITE_CONFIG=/path/to/config.toml jarvis-lite
```
