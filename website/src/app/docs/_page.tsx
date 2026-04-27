'use client';

import { useState } from 'react';
import { Header } from '@/components/Header';
import { Footer } from '@/components/Footer';
import { 
  BookOpen, 
  Download, 
  Wrench, 
  Question, 
  GithubLogo,
  ArrowRight,
  Copy,
  Check,
  LinuxLogo,
  WindowsLogo,
  AppleLogo
} from '@phosphor-icons/react';
import Link from 'next/link';
import { INSTALL_COMMANDS, GITHUB_URL } from '@/lib/constants';
import { cn } from '@/lib/utils';

const DOC_SECTIONS = [
  {
    id: 'getting-started',
    title: 'Getting Started',
    icon: BookOpen,
    content: `OpenSoundDeck is a cross-platform soundboard application that injects audio directly into your microphone input. This allows you to play sounds that others hear as if they were coming from your microphone.`,
  },
  {
    id: 'installation',
    title: 'Installation',
    icon: Download,
    content: `Choose your platform below to see installation instructions.`,
  },
  {
    id: 'configuration',
    title: 'Configuration',
    icon: Wrench,
    content: `After installation, you need to configure your audio devices.`,
  },
  {
    id: 'troubleshooting',
    title: 'Troubleshooting',
    icon: Question,
    content: `Common issues and their solutions.`,
  },
];

const BUILD_COMMANDS = {
  linux: `# Ubuntu/Debian
sudo apt update
sudo apt install build-essential git cmake
sudo apt install qt6-base-dev qt6-multimedia-dev libqt6network6
sudo apt install libasound2-dev libpulse-dev libx11-dev

# Clone and build
git clone https://github.com/kkrugley/OpenSoundDeck.git
cd OpenSoundDeck
git checkout dev
mkdir build && cd build
cmake ..
cmake --build . --config Release`,
  windows: `# Prerequisites:
# - Visual Studio 2019 or later
# - Qt 6 installed
# - CMake

# Clone repository
git clone https://github.com/kkrugley/OpenSoundDeck.git
cd OpenSoundDeck
git checkout dev

# Build with Visual Studio
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release`,
  macos: `# Prerequisites:
# - Xcode Command Line Tools
# - Homebrew

# Install dependencies
brew install cmake qt@6

# Clone and build
git clone https://github.com/kkrugley/OpenSoundDeck.git
cd OpenSoundDeck
git checkout dev
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6
cmake --build . --config Release`,
};

export default function DocsPage() {
  const [currentLang, setCurrentLang] = useState('en');
  const [activeSection, setActiveSection] = useState('getting-started');
  const [activePlatform, setActivePlatform] = useState<'linux' | 'windows' | 'macos'>('linux');
  const [copied, setCopied] = useState<string | null>(null);

  const handleCopy = async (text: string, id: string) => {
    await navigator.clipboard.writeText(text);
    setCopied(id);
    setTimeout(() => setCopied(null), 2000);
  };

  return (
    <div className="flex min-h-screen flex-col">
      <Header currentLang={currentLang} onLanguageChange={setCurrentLang} />
      
      <main className="flex-1">
        <div className="mx-auto max-w-7xl px-4 sm:px-6 lg:px-8">
          <div className="flex flex-col gap-8 py-8 lg:flex-row">
            {/* Sidebar */}
            <aside className="lg:w-64 lg:flex-shrink-0">
              <div className="sticky top-24">
                <h2 className="mb-4 text-sm font-semibold uppercase tracking-wider text-[var(--text-tertiary)]">
                  Documentation
                </h2>
                <nav className="space-y-1">
                  {DOC_SECTIONS.map((section) => {
                    const Icon = section.icon;
                    return (
                      <button
                        key={section.id}
                        onClick={() => setActiveSection(section.id)}
                        className={cn(
                          "flex w-full items-center gap-3 rounded-md px-3 py-2 text-left text-sm transition-colors",
                          activeSection === section.id
                            ? "bg-[var(--code-bg)] font-medium text-[var(--text-primary)]"
                            : "text-[var(--text-secondary)] hover:bg-[var(--code-bg)] hover:text-[var(--text-primary)]"
                        )}
                      >
                        <Icon className="h-4 w-4" />
                        {section.title}
                      </button>
                    );
                  })}
                </nav>

                <div className="mt-8 border-t border-[var(--border)] pt-6">
                  <h2 className="mb-4 text-sm font-semibold uppercase tracking-wider text-[var(--text-tertiary)]">
                    Resources
                  </h2>
                  <nav className="space-y-1">
                    <Link
                      href={GITHUB_URL}
                      target="_blank"
                      rel="noopener noreferrer"
                      className="flex items-center gap-3 rounded-md px-3 py-2 text-sm text-[var(--text-secondary)] hover:bg-[var(--code-bg)] hover:text-[var(--text-primary)]"
                    >
                      <GithubLogo className="h-4 w-4" />
                      GitHub Repository
                    </Link>
                    <Link
                      href="/download"
                      className="flex items-center gap-3 rounded-md px-3 py-2 text-sm text-[var(--text-secondary)] hover:bg-[var(--code-bg)] hover:text-[var(--text-primary)]"
                    >
                      <Download className="h-4 w-4" />
                      Downloads
                    </Link>
                  </nav>
                </div>
              </div>
            </aside>

            {/* Main Content */}
            <div className="flex-1">
              {/* Getting Started */}
              {activeSection === 'getting-started' && (
                <div className="space-y-8">
                  <div>
                    <h1 className="text-3xl font-bold text-[var(--text-primary)]">
                      Getting Started
                    </h1>
                    <p className="mt-4 text-[var(--text-secondary)]">
                      OpenSoundDeck is a cross-platform soundboard application that injects 
                      audio directly into your microphone input. This allows you to play sounds 
                      that others hear as if they were coming from your microphone.
                    </p>
                  </div>

                  <div className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
                    <h2 className="text-lg font-bold text-[var(--text-primary)]">
                      What is OpenSoundDeck?
                    </h2>
                    <ul className="mt-4 space-y-3 text-[var(--text-secondary)]">
                      <li className="flex items-start gap-2">
                        <span className="text-[var(--success)]">[+]</span>
                        <span>Play sound effects, music, or audio clips through your microphone</span>
                      </li>
                      <li className="flex items-start gap-2">
                        <span className="text-[var(--success)]">[+]</span>
                        <span>Works with Discord, Zoom, Teams, and any voice chat application</span>
                      </li>
                      <li className="flex items-start gap-2">
                        <span className="text-[var(--success)]">[+]</span>
                        <span>Global hotkeys to trigger sounds from anywhere (even while gaming)</span>
                      </li>
                      <li className="flex items-start gap-2">
                        <span className="text-[var(--success)]">[+]</span>
                        <span>Cross-platform: Linux, Windows, and macOS</span>
                      </li>
                      <li className="flex items-start gap-2">
                        <span className="text-[var(--success)]">[+]</span>
                        <span>100% free and open-source</span>
                      </li>
                    </ul>
                  </div>

                  <div className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
                    <h2 className="text-lg font-bold text-[var(--text-primary)]">
                      System Requirements
                    </h2>
                    <ul className="mt-4 space-y-2 text-[var(--text-secondary)]">
                      <li><strong>Linux:</strong> PulseAudio or PipeWire</li>
                      <li><strong>Windows:</strong> Windows 10/11 with VB-Audio Virtual Cable</li>
                      <li><strong>macOS:</strong> macOS 11+ with BlackHole</li>
                    </ul>
                  </div>
                </div>
              )}

              {/* Installation */}
              {activeSection === 'installation' && (
                <div className="space-y-8">
                  <div>
                    <h1 className="text-3xl font-bold text-[var(--text-primary)]">
                      Installation
                    </h1>
                    <p className="mt-4 text-[var(--text-secondary)]">
                      Choose your platform below to see installation instructions.
                    </p>
                  </div>

                  {/* Platform Tabs */}
                  <div className="flex gap-2 border-b border-[var(--border)]">
                    {[
                      { id: 'linux', label: 'Linux', icon: LinuxLogo },
                      { id: 'windows', label: 'Windows', icon: WindowsLogo },
                      { id: 'macos', label: 'macOS', icon: AppleLogo },
                    ].map((platform) => {
                      const Icon = platform.icon;
                      return (
                        <button
                          key={platform.id}
                          onClick={() => setActivePlatform(platform.id as typeof activePlatform)}
                          className={cn(
                            "flex items-center gap-2 px-4 py-2 text-sm font-medium transition-colors",
                            activePlatform === platform.id
                              ? "border-b-2 border-[var(--text-primary)] text-[var(--text-primary)]"
                              : "text-[var(--text-tertiary)] hover:text-[var(--text-secondary)]"
                          )}
                        >
                          <Icon className="h-4 w-4" weight="bold" />
                          {platform.label}
                        </button>
                      );
                    })}
                  </div>

                  {/* Build Instructions */}
                  <div className="relative">
                    <pre className="overflow-x-auto rounded-lg border border-[var(--border)] bg-[var(--background)] p-4 text-xs text-[var(--text-primary)]">
                      <code>{BUILD_COMMANDS[activePlatform]}</code>
                    </pre>
                    <button
                      onClick={() => handleCopy(BUILD_COMMANDS[activePlatform], activePlatform)}
                      className="absolute right-3 top-3 rounded-md p-2 text-[var(--text-tertiary)] hover:bg-[var(--code-bg)] hover:text-[var(--text-primary)]"
                    >
                      {copied === activePlatform ? (
                        <Check className="h-4 w-4 text-[var(--success)]" weight="bold" />
                      ) : (
                        <Copy className="h-4 w-4" weight="bold" />
                      )}
                    </button>
                  </div>

                  {/* Platform-specific notes */}
                  {activePlatform === 'linux' && (
                    <div className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
                      <h3 className="font-bold text-[var(--text-primary)]">Linux Notes</h3>
                      <p className="mt-2 text-sm text-[var(--text-secondary)]">
                        No virtual audio driver is needed on Linux. OpenSoundDeck automatically 
                        creates virtual sinks via PulseAudio or PipeWire.
                      </p>
                    </div>
                  )}

                  {activePlatform === 'windows' && (
                    <div className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
                      <h3 className="font-bold text-[var(--text-primary)]">Windows Requirements</h3>
                      <p className="mt-2 text-sm text-[var(--text-secondary)]">
                        You must install VB-Audio Virtual Cable before running OpenSoundDeck:
                      </p>
                      <ol className="mt-2 list-inside list-decimal text-sm text-[var(--text-secondary)]">
                        <li>Download from <a href="https://vb-audio.com/Cable/" target="_blank" rel="noopener noreferrer" className="underline">vb-audio.com</a></li>
                        <li>Run VBCABLE_Setup.exe</li>
                        <li>Reboot your system</li>
                      </ol>
                    </div>
                  )}

                  {activePlatform === 'macos' && (
                    <div className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
                      <h3 className="font-bold text-[var(--text-primary)]">macOS Requirements</h3>
                      <p className="mt-2 text-sm text-[var(--text-secondary)]">
                        You must install BlackHole virtual audio driver:
                      </p>
                      <code className="mt-2 block rounded bg-[var(--background)] p-2 text-xs">
                        brew install blackhole-2ch
                      </code>
                      <p className="mt-2 text-sm text-[var(--text-secondary)]">
                        After installation, approve the driver in System Preferences → Security & Privacy, then reboot.
                      </p>
                    </div>
                  )}
                </div>
              )}

              {/* Configuration */}
              {activeSection === 'configuration' && (
                <div className="space-y-8">
                  <div>
                    <h1 className="text-3xl font-bold text-[var(--text-primary)]">
                      Configuration
                    </h1>
                    <p className="mt-4 text-[var(--text-secondary)]">
                      After installation, configure your audio devices in OpenSoundDeck settings.
                    </p>
                  </div>

                  <div className="space-y-6">
                    <div className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
                      <h2 className="text-lg font-bold text-[var(--text-primary)]">
                        Audio Devices Setup
                      </h2>
                      <ol className="mt-4 list-inside list-decimal space-y-3 text-[var(--text-secondary)]">
                        <li>Open OpenSoundDeck settings</li>
                        <li>Select your microphone as <strong>Input Device</strong></li>
                        <li>Select virtual cable as <strong>Output Device</strong></li>
                        <li>Select your headphones/speakers as <strong>Monitor Device</strong></li>
                        <li>Adjust volume levels for each device</li>
                      </ol>
                    </div>

                    <div className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
                      <h2 className="text-lg font-bold text-[var(--text-primary)]">
                        Adding Sounds
                      </h2>
                      <ol className="mt-4 list-inside list-decimal space-y-3 text-[var(--text-secondary)]">
                        <li>Click the &quot;+&quot; button in the main window</li>
                        <li>Select an audio file (MP3, WAV, OGG, or FLAC)</li>
                        <li>Assign a hotkey by clicking the hotkey field and pressing your desired key combination</li>
                        <li>Test the sound by clicking the play button or pressing the hotkey</li>
                      </ol>
                    </div>

                    <div className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
                      <h2 className="text-lg font-bold text-[var(--text-primary)]">
                        Hotkey Configuration
                      </h2>
                      <p className="mt-2 text-[var(--text-secondary)]">
                        Global hotkeys work even when OpenSoundDeck is not focused. 
                        Popular combinations include:
                      </p>
                      <ul className="mt-4 space-y-2 text-[var(--text-secondary)]">
                        <li><code>Ctrl+Shift+1</code> — Play first sound</li>
                        <li><code>Ctrl+Shift+2</code> — Play second sound</li>
                        <li><code>F13-F24</code> — Media keys (if available)</li>
                      </ul>
                    </div>
                  </div>
                </div>
              )}

              {/* Troubleshooting */}
              {activeSection === 'troubleshooting' && (
                <div className="space-y-8">
                  <div>
                    <h1 className="text-3xl font-bold text-[var(--text-primary)]">
                      Troubleshooting
                    </h1>
                    <p className="mt-4 text-[var(--text-secondary)]">
                      Common issues and their solutions.
                    </p>
                  </div>

                  <div className="space-y-6">
                    {[
                      {
                        problem: 'OpenSoundDeck cannot find virtual audio device',
                        solution: 'Ensure you have installed the required virtual audio driver (VB-Cable on Windows, BlackHole on macOS). On Linux, check that PulseAudio or PipeWire is running.',
                      },
                      {
                        problem: 'Sounds are not playing in voice chat',
                        solution: 'In your voice chat application (Discord, Zoom, etc.), set the virtual cable as your microphone input device.',
                      },
                      {
                        problem: 'Hotkeys are not working',
                        solution: 'Some applications may capture hotkeys before OpenSoundDeck can receive them. Try using different key combinations or run OpenSoundDeck with elevated permissions.',
                      },
                      {
                        problem: 'Audio quality is poor or distorted',
                        solution: 'Check your audio sample rate settings. Ensure all devices (microphone, virtual cable, output) are using the same sample rate (e.g., 48000 Hz).',
                      },
                      {
                        problem: 'Build fails with CMake errors',
                        solution: 'Ensure Qt6 is properly installed and accessible. You may need to specify the Qt path: cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/compiler',
                      },
                    ].map((item, index) => (
                      <div 
                        key={index}
                        className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6"
                      >
                        <h3 className="font-bold text-[var(--text-primary)]">
                          {item.problem}
                        </h3>
                        <p className="mt-2 text-[var(--text-secondary)]">
                          {item.solution}
                        </p>
                      </div>
                    ))}
                  </div>

                  <div className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
                    <h2 className="text-lg font-bold text-[var(--text-primary)]">
                      Still need help?
                    </h2>
                    <p className="mt-2 text-[var(--text-secondary)]">
                      If you cannot find a solution here, please:
                    </p>
                    <ul className="mt-4 space-y-2 text-[var(--text-secondary)]">
                      <li>
                        <Link href="/help" className="inline-flex items-center gap-1 text-[var(--text-primary)] underline">
                          Contact us <ArrowRight className="h-3 w-3" />
                        </Link>
                      </li>
                      <li>
                        <Link href={`${GITHUB_URL}/issues`} target="_blank" rel="noopener noreferrer" className="inline-flex items-center gap-1 text-[var(--text-primary)] underline">
                          Open a GitHub issue <ArrowRight className="h-3 w-3" />
                        </Link>
                      </li>
                    </ul>
                  </div>
                </div>
              )}
            </div>
          </div>
        </div>
      </main>
      
      <Footer currentLang={currentLang} onLanguageChange={setCurrentLang} />
    </div>
  );
}
