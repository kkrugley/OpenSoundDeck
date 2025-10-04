
---

# 🔊 OpenSoundDeck

**Your cross-platform, open-source soundboard for voice chats and streams. Inject high-quality audio directly into your microphone input, triggered by global hotkeys.**

---

## ✨ What is OpenSoundDeck?

OpenSoundDeck is a free and open-source application that allows you to play sounds through your microphone input for others to hear. Whether you're gaming with friends, live-streaming, or on a conference call, you can play sound effects, music, or clips at the press of a button.

This project was born out of a love for tools like Soundpad but with a desire for a solution that is **truly cross-platform (Linux, Windows, macOS)** and built by the community, for the community. No ads, no paywalls, just pure functionality.

## 🚀 Core Features

*   🎤 **Mic Injection:** Plays audio files so they sound like they're coming from your microphone.
*   ⌨️ **Global Hotkeys:** Assign system-wide hotkeys to your favorite sounds so you can trigger them from anywhere, even inside a game.
*   🖥️ **Cross-Platform:** A single native application that runs on Linux, Windows, and macOS.
*   🎧 **Sound & Mic Mixing:** Your voice and the played audio are mixed together seamlessly. Your friends will still hear you speak!
*   🎛️ **Simple UI:** An easy-to-use interface to manage your sound library and configure devices.
*   ❤️ **100% Free & Open-Source:** Built with transparency and a passion for great software.

## 🌱 Why Another Soundboard?

We believe essential tools should be accessible to everyone, regardless of their operating system. OpenSoundDeck is built on these principles:

*   **Community-Driven:** The future of this project is in the hands of its users. Every feature request, bug report, and contribution matters.
*   **Transparency:** All code is open for review. You know exactly what's running on your machine.
*   **Performance:** Written in C++ with the Qt framework for a responsive, native experience with minimal latency.
*   **Freedom:** Free as in "free speech" and free as in "free beer."

## 🚧 Project Status

This project is currently in **active development**. While the core functionality is being built, we are looking for passionate people to help us shape the future of OpenSoundDeck. Check the [Projects](https://github.com/kkrugley/OpenSoundDeck/projects) tab to see our roadmap!

## 📦 Getting Started

*Final installation packages are not yet available. Here is the intended plan:*

### For Users
*   **Linux**: We will provide an `AppImage` for easy, no-install usage, and a `Flatpak` for seamless integration with your desktop environment.
*   **Windows**: A standard `.msi` installer will be available.
*   **macOS**: A `.dmg` file that you can drag and drop into your Applications folder.

### For Developers
1.  Clone the repository:
    ```bash
    git clone https://github.com/kkrugley/OpenSoundDeck.git
    ```
2.  Follow the build instructions in `BUILDING.md` (coming soon).

## ❤️ How to Contribute

**This is where you come in!** OpenSoundDeck can only succeed with your help. We welcome contributions of all kinds, not just code.

*   💡 **Suggest a Feature:** Have a great idea? Open an [issue](https://github.com/kkrugley/OpenSoundDeck/issues) and tell us about it!
*   🐛 **Report a Bug:** If you find something that isn't working right, letting us know is one of the most helpful things you can do.
*   ✍️ **Write Code:** Grab an open issue, fork the repo, and submit a pull request. Whether it's a small fix or a major new feature, all contributions are valued.
*   📚 **Improve Documentation:** Help us make our README and other documentation clearer and more helpful.
*   🌐 **Translate the App:** Help make OpenSoundDeck accessible to people all over the world.

Please read our `CONTRIBUTING.md` (coming soon) for guidelines and best practices.

## 🛠️ Technology Stack

*   **Core Logic:** C++20
*   **GUI Framework:** Qt 6
*   **Build System:** CMake
*   **Audio Backend:** Miniaudio (a fantastic cross-platform audio library)
*   **CI/CD:** GitHub Actions

## 📄 License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details. You are free to use, modify, and distribute this software.
