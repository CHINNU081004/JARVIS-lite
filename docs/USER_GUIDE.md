# User Guide

## Start

```sh
jarvis-lite
```

Type `/help` to see commands.

Start the basic menu UI:

```sh
jarvis-lite --ui
```

## Chat

Enter normal text at the prompt. Responses stream through the inference interface.

## Models

JARVIS Lite lists curated GGUF models for multimodal and coding workflows. Models are large and are downloaded separately.

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
/models download gemma-4-e2b-it-q4
```

Select a model:

```text
/models select gemma-4-e2b-it-q4
```

Recommended Gemma 4 model IDs:

```text
gemma-4-e2b-it-q4       Smallest default, about 3.35 GiB download
gemma-4-e4b-it-q4       Better quality, about 5.15 GiB download
gemma-4-12b-it-q4       Larger desktop model, about 6.98 GiB download
gemma-4-26b-a4b-it-q4   Mixture-of-experts desktop model, about 14.4 GiB download
gemma-4-31b-it-q4       Largest dense model, about 17.7 GiB download
```

Coding model IDs:

```text
qwen2.5-coder-7b-instruct-q4-k-m
deepseek-coder-v2-lite-base-iq4-xs
```

Download any direct GGUF URL:

```text
/models download-url my-model https://huggingface.co/user/repo/resolve/main/model.gguf
/models select my-model
```

If `/models download ...` fails with an HTTP error, open the model page on Hugging Face in your browser, accept any required terms, set `HF_TOKEN`, then try again.

Linux example:

```sh
jarvis-lite
```

Inside JARVIS Lite:

```text
/models available
/models download gemma-4-e2b-it-q4
/models select gemma-4-e2b-it-q4
hello
```

Windows PowerShell example:

```powershell
.\jarvis-lite-windows-x64.exe
```

Inside JARVIS Lite:

```text
/models available
/models download gemma-4-e2b-it-q4
/models select gemma-4-e2b-it-q4
hello
```

This application can download and track Gemma 4 GGUF model files, but the current native inference engine is still a scaffold until a GGUF runtime is linked.

## Voice

Show voice runtime status:

```text
/voice status
```

Speech-to-text command:

```text
/voice stt speech.wav
```

Text-to-speech command:

```text
/voice tts hello from JARVIS Lite
```

Voice conversation from an audio file:

```text
/voice chat speech.wav
```

Configure speech-to-text with a command that prints the transcript to stdout. Use `{audio}` where the audio file path should be inserted:

```sh
export JARVIS_STT_COMMAND="whisper-cli -m /path/to/ggml-base.en.bin -f {audio} --no-timestamps"
```

Configure text-to-speech with a command that speaks text. Use `{text}` where the response text should be inserted:

```sh
export JARVIS_TTS_COMMAND="piper --model /path/to/voice.onnx --output-raw <<< {text} | aplay -r 22050 -f S16_LE -t raw -"
```

If no `JARVIS_TTS_COMMAND` is set, JARVIS Lite tries common system speech commands such as `espeak-ng`, `espeak`, or `spd-say` when available.

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
