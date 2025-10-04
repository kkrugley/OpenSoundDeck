# ❤️ Contributing to OpenSoundDeck

Thank you for your interest in making OpenSoundDeck better! We are thrilled to have you here. This is a community-driven project, and every contribution, no matter how small, is incredibly valuable.

This document provides guidelines for contributing. Following them helps us maintain a high standard of quality and makes the process smoother for everyone.


## Git Workflow

We use a `dev` branch for all development. The `main` branch contains only stable, tested, release-ready code. **All pull requests must be made against the `dev` branch.**

## How Can I Contribute?

*   🐛 **Report a Bug:** Submit a detailed report in the [issues](https://github.com/kkrugley/OpenSoundDeck/issues) tab.
*   💡 **Suggest an Enhancement:** Propose a new feature or improvement.
*   ✍️ **Write Code:** Fix a bug or implement a new feature.
*   📚 **Improve Documentation:** Help us make our guides clearer and more helpful.
*   🌐 **Translate the App:** Make OpenSoundDeck accessible to users around the world.

## 🚀 Submitting a Pull Request

Ready to contribute code? Awesome! Here's how to do it right.

1.  **Fork the repository** to your own GitHub account.

2.  **Clone your fork** to your local machine.
    ```bash
    git clone https://github.com/kkrugley/OpenSoundDeck.git
    cd OpenSoundDeck
    ```

3.  **Switch to the `dev` branch** and make sure it's up to date.
    ```bash
    git checkout dev
    git pull upstream dev  # 'upstream' should be a remote pointing to the main repository
    ```

4.  **Create a new branch** for your changes, starting from `dev`. Use a descriptive name (e.g., `feature/sound-editor` or `fix/hotkey-crash`).
    ```bash
    git checkout -b feature/your-amazing-feature
    ```

5.  **Make your changes.** Please follow the development guidelines below.

6.  **Build the project and run the tests** to ensure you haven't broken anything.
    ```bash
    # from your build directory
    cmake --build .
    ctest
    ```

7.  **Commit your changes** with a clear and concise commit message.

8.  **Push your branch** to your fork on GitHub.
    ```bash
    git push origin feature/your-amazing-feature
    ```

9.  **Open a Pull Request (PR)** from your feature branch to the **`dev` branch** of the original repository.
    *   In the PR description, clearly explain the changes you made and link to the issue it resolves (e.g., `Closes #42`).

## ⭐ Development Guidelines

To ensure code quality and consistency, please adhere to the following rules.

### **1. Testing is Mandatory**
Quality is not an afterthought. Every new feature or bug fix must be accompanied by tests.
*   **New Features:** Must include unit tests that cover the new functionality.
*   **Bug Fixes:** Must include a test that fails without the fix and passes with it. This prevents regressions.
*   All tests must pass before a PR will be considered for merging.

### **2. Internationalization (i18n)**
We want OpenSoundDeck to be usable by everyone.
*   All user-facing strings in the UI must be wrapped in `tr()`.
*   **Good:** `button->setText(tr("Save"));`
*   **Bad:** `button->setText("Save");`
*   This allows translators to easily create language files for the application.

### **3. Coding Style**
*   Please follow the existing coding style in the project.
*   Use descriptive variable and function names.
*   Keep your code clean, readable, and well-commented where necessary.

Thank you for helping us build something amazing!