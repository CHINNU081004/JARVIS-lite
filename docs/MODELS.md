# Model Guide

JARVIS Lite can download built-in GGUF models and any direct GGUF URL.

The current native inference engine is still a scaffold. These commands install model files and select them in the config, but real token generation requires linking a GGUF runtime such as llama.cpp in a later build.

## Built-In Models

Use `/models available` inside JARVIS Lite to see hardware availability on your machine.

```text
gemma-4-e2b-it-q4              Multimodal starter model for laptops, about 3.35 GiB
gemma-4-e4b-it-q4              Balanced Gemma 4 multimodal model, about 5.15 GiB
gemma-4-12b-it-q4              Larger Gemma 4 unified model, about 6.98 GiB
gemma-4-26b-a4b-it-q4          High-end Gemma 4 MoE model, about 14.4 GiB
gemma-4-31b-it-q4              Largest Gemma 4 dense model, about 17.7 GiB
qwen2.5-coder-7b-instruct-q4-k-m Compact coding model, about 4.68 GiB
deepseek-coder-v2-lite-base-iq4-xs Larger coding model, about 8.65 GiB
```

## Download A Built-In Model

Start the app:

```sh
jarvis-lite
```

Then run:

```text
/models available
/models download gemma-4-e2b-it-q4
/models select gemma-4-e2b-it-q4
```

## Download Any GGUF Model

Use `/models download-url` with a simple local ID and a direct model file URL:

```text
/models download-url my-model https://huggingface.co/user/repo/resolve/main/model.gguf
/models select my-model
```

The model ID is saved as `<id>.gguf` under the configured model directory. Use letters, numbers, dots, dashes, and underscores only.

For a local file:

```text
/models download-url my-local-model file:///home/chinnu/Downloads/model.gguf
/models select my-local-model
```

## Hugging Face Access

Some model pages require accepting terms before downloads work. Open the model page in your browser, accept the terms, create a Hugging Face access token, then start JARVIS Lite with `HF_TOKEN` set.

Linux:

```sh
export HF_TOKEN="hf_your_token_here"
jarvis-lite
```

Windows PowerShell:

```powershell
$env:HF_TOKEN = "hf_your_token_here"
.\jarvis-lite-windows-x64.exe
```

## Voice Models

The app has STT/TTS commands and can call external speech tools:

```text
/voice status
/voice stt speech.wav
/voice tts hello
/voice chat speech.wav
```

Set `JARVIS_STT_COMMAND` for transcription and `JARVIS_TTS_COMMAND` for speech output. The placeholders `{audio}` and `{text}` are replaced safely with the audio path or response text.
