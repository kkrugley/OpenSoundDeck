# 🔊 OpenSoundDeck

Cross-platform, open-source soundboard for voice chats and streams. Inject high-quality audio directly into your microphone input, triggered by global hotkeys.

---

## What is OpenSoundDeck?

OpenSoundDeck lets you play sounds through your microphone. When you hit a hotkey, a sound file gets mixed into your mic input—so Discord, Zoom, Teams, or any game hears it coming from you. No setup needed beyond telling the app which audio devices to use.

It started as a frustrated attempt to replace Soundpad on Linux. Most soundboards lock you into one OS or ask for money. This one works everywhere and costs nothing.

## Features

- **Mic injection** — audio plays through your microphone so others hear it naturally
- **Global hotkeys** — trigger sounds from anywhere, even inside games
- **Cross-platform** — Linux, Windows, macOS all work the same way
- **Low latency** — written in C++20 with minimal audio delay
- **Simple to use** — drag audio files in, assign hotkeys, done
- **100% free and open source** — no ads, no licensing nonsense

## Why another soundboard?

Most soundboards only work on Windows. The open-source ones are scattered and often unmaintained. We wanted something that:

- Runs natively on Linux, Windows, and macOS without workarounds
- Doesn't phone home or track what you're doing
- Has code you can read and modify
- Stays simple—no bloat, no upsells

## Project Status

OpenSoundDeck is in active development. The core is working—you can use it now—but we're still adding features and fixing edge cases. See the [GitHub Projects tab](https://github.com/kkrugley/OpenSoundDeck/projects) for what's in progress.

## Getting Started

**Note:** No binary releases yet. You have to build from source.

1. Clone the repository:
   ```bash
   git clone https://github.com/kkrugley/OpenSoundDeck.git
   ```
2. Follow the [build instructions in BUILDING.md](BUILDING.md).

## Contributing

We need help. Code contributions are great, but so are bug reports, documentation fixes, and translations. Start by checking [existing issues](https://github.com/kkrugley/OpenSoundDeck/issues) or opening a new one.

## Built With

- **C++20** — core logic
- **Qt 6** — GUI framework
- **CMake** — build system
- **Miniaudio** — cross-platform audio library
- **GitHub Actions** — CI/CD

## License

GPL-3.0. See [LICENSE](LICENSE).
