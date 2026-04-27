'use client';

import { useState } from 'react';
import { Header } from '@/components/Header';
import { Footer } from '@/components/Footer';
import { 
  LinuxLogo, 
  WindowsLogo, 
  AppleLogo,
  Download,
  Archive,
  Package,
  Copy,
  Check,
  ArrowRight
} from '@phosphor-icons/react';
import Link from 'next/link';
import { PLATFORMS } from '@/lib/constants';
import { cn } from '@/lib/utils';

const DOWNLOAD_OPTIONS = [
  {
    platform: 'linux',
    name: 'Linux',
    icon: LinuxLogo,
    color: '#fcc624',
    downloads: [
      { name: 'AppImage', url: '#', size: '45 MB', type: 'appimage' },
      { name: 'Flatpak', url: '#', size: '50 MB', type: 'flatpak' },
      { name: 'tar.gz', url: '#', size: '40 MB', type: 'archive' },
    ],
    installCommand: 'flatpak install opensounddeck',
  },
  {
    platform: 'windows',
    name: 'Windows',
    icon: WindowsLogo,
    color: '#0078d4',
    downloads: [
      { name: 'Installer (.msi)', url: '#', size: '55 MB', type: 'installer' },
      { name: 'Portable (.exe)', url: '#', size: '52 MB', type: 'portable' },
    ],
    installCommand: 'winget install OpenSoundDeck',
  },
  {
    platform: 'macos',
    name: 'macOS',
    icon: AppleLogo,
    color: '#a2aaad',
    downloads: [
      { name: 'Disk Image (.dmg)', url: '#', size: '48 MB', type: 'dmg' },
      { name: 'Homebrew', url: '#', size: '—', type: 'brew' },
    ],
    installCommand: 'brew install opensounddeck',
  },
];

export default function DownloadPage() {
  const [currentLang, setCurrentLang] = useState('en');
  const [activeTab, setActiveTab] = useState<'linux' | 'windows' | 'macos'>('linux');
  const [copied, setCopied] = useState(false);

  const activePlatform = DOWNLOAD_OPTIONS.find(p => p.platform === activeTab);

  const handleCopy = async (text: string) => {
    await navigator.clipboard.writeText(text);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const getDownloadIcon = (type: string) => {
    switch (type) {
      case 'appimage':
      case 'portable':
        return Package;
      case 'archive':
      case 'tar':
        return Archive;
      default:
        return Download;
    }
  };

  return (
    <div className="flex min-h-screen flex-col">
      <Header currentLang={currentLang} onLanguageChange={setCurrentLang} />
      
      <main className="flex-1">
        {/* Page Header */}
        <section className="w-full border-b border-[var(--border)] py-16">
          <div className="mx-auto max-w-4xl px-4 text-center sm:px-6 lg:px-8">
            <h1 className="text-3xl font-bold text-[var(--text-primary)] sm:text-4xl">
              Download OpenSoundDeck
            </h1>
            <p className="mx-auto mt-4 max-w-2xl text-[var(--text-secondary)]">
              Choose your platform below to download the latest version of OpenSoundDeck. 
              All downloads are free and open-source.
            </p>
          </div>
        </section>

        {/* Beta Notice */}
        <section className="w-full border-b border-[var(--border)] bg-[var(--warning)]/5 py-6">
          <div className="mx-auto max-w-4xl px-4 sm:px-6 lg:px-8">
            <div className="rounded-lg border border-[var(--warning)]/30 bg-[var(--warning)]/10 p-4">
              <h3 className="flex items-center gap-2 font-bold text-[var(--text-primary)]">
                <span className="text-lg">⚠️</span> No Binary Releases Yet
              </h3>
              <p className="mt-2 text-sm text-[var(--text-secondary)]">
                Binary releases are not yet available. You'll need to build OpenSoundDeck from source.
                Follow the <Link href="/docs#installation" className="underline hover:text-[var(--text-primary)]">
                  step-by-step build instructions
                </Link> in our documentation to get started.
              </p>
            </div>
          </div>
        </section>

        {/* Platform Tabs */}
        <section className="w-full border-b border-[var(--border)] py-8">
          <div className="mx-auto max-w-4xl px-4 sm:px-6 lg:px-8">
            <div className="flex justify-center gap-2">
              {DOWNLOAD_OPTIONS.map((platform) => {
                const Icon = platform.icon;
                return (
                  <button
                    key={platform.platform}
                    onClick={() => setActiveTab(platform.platform as typeof activeTab)}
                    className={cn(
                      "flex items-center gap-2 rounded-md px-6 py-3 text-sm font-medium transition-all",
                      activeTab === platform.platform
                        ? "bg-[var(--accent-dark)] text-[var(--accent-light)]"
                        : "bg-[var(--code-bg)] text-[var(--text-secondary)] hover:bg-[var(--border)] hover:text-[var(--text-primary)]"
                    )}
                  >
                    <Icon 
                      className="h-5 w-5" 
                      weight="bold"
                      style={{ color: activeTab === platform.platform ? 'inherit' : platform.color }}
                    />
                    {platform.name}
                  </button>
                );
              })}
            </div>
          </div>
        </section>

        {/* Download Options */}
        <section className="w-full py-16">
          <div className="mx-auto max-w-4xl px-4 sm:px-6 lg:px-8">
            {activePlatform && (
              <div className="space-y-8">
                {/* Download Cards */}
                <div className="grid gap-4 sm:grid-cols-2 lg:grid-cols-3">
                  {activePlatform.downloads.map((download) => {
                    const Icon = getDownloadIcon(download.type);
                    return (
                      <a
                        key={download.name}
                        href={download.url}
                        className="group flex flex-col rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6 transition-all hover:border-[var(--text-primary)]"
                      >
                        <div className="flex items-start justify-between">
                          <Icon 
                            className="h-8 w-8 transition-colors group-hover:text-[var(--text-primary)]" 
                            weight="bold"
                            style={{ color: activePlatform.color }}
                          />
                          <span className="text-xs text-[var(--text-tertiary)]">
                            {download.size}
                          </span>
                        </div>
                        <h3 className="mt-4 font-bold text-[var(--text-primary)]">
                          {download.name}
                        </h3>
                        <span className="mt-2 inline-flex items-center gap-1 text-sm text-[var(--text-secondary)] group-hover:text-[var(--text-primary)]">
                          Download
                          <ArrowRight className="h-3 w-3 transition-transform group-hover:translate-x-1" />
                        </span>
                      </a>
                    );
                  })}
                </div>

                {/* Package Manager Install */}
                <div className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
                  <h3 className="font-bold text-[var(--text-primary)]">
                    Or install via package manager
                  </h3>
                  <div className="mt-4 flex items-center gap-2">
                    <code className="flex-1 rounded-md bg-[var(--background)] px-4 py-3 text-sm text-[var(--text-primary)]">
                      {activePlatform.installCommand}
                    </code>
                    <button
                      onClick={() => handleCopy(activePlatform.installCommand)}
                      className="flex-shrink-0 rounded-md border border-[var(--border)] p-3 text-[var(--text-tertiary)] hover:bg-[var(--border)] hover:text-[var(--text-primary)]"
                    >
                      {copied ? (
                        <Check className="h-4 w-4 text-[var(--success)]" weight="bold" />
                      ) : (
                        <Copy className="h-4 w-4" weight="bold" />
                      )}
                    </button>
                  </div>
                </div>

                {/* Platform-specific notes */}
                {activeTab === 'windows' && (
                  <div className="rounded-lg border-l-4 border-[var(--success)] bg-[var(--success)]/10 p-4">
                    <h4 className="font-bold text-[var(--text-primary)]">
                      Windows Requirements
                    </h4>
                    <p className="mt-2 text-sm text-[var(--text-secondary)]">
                      You need to install VB-Audio Virtual Cable before using OpenSoundDeck. 
                      The app will guide you through setup on first run.
                    </p>
                    <a 
                      href="https://vb-audio.com/Cable/" 
                      target="_blank" 
                      rel="noopener noreferrer"
                      className="mt-2 inline-flex items-center gap-1 text-sm text-[var(--text-primary)] underline"
                    >
                      Download VB-Cable <ArrowRight className="h-3 w-3" />
                    </a>
                  </div>
                )}

                {activeTab === 'macos' && (
                  <div className="rounded-lg border-l-4 border-[var(--success)] bg-[var(--success)]/10 p-4">
                    <h4 className="font-bold text-[var(--text-primary)]">
                      macOS Requirements
                    </h4>
                    <p className="mt-2 text-sm text-[var(--text-secondary)]">
                      You need to install BlackHole virtual audio driver. 
                      Run <code>brew install blackhole-2ch</code> or download manually.
                    </p>
                    <a 
                      href="https://github.com/ExistentialAudio/BlackHole" 
                      target="_blank" 
                      rel="noopener noreferrer"
                      className="mt-2 inline-flex items-center gap-1 text-sm text-[var(--text-primary)] underline"
                    >
                      Download BlackHole <ArrowRight className="h-3 w-3" />
                    </a>
                  </div>
                )}

                {activeTab === 'linux' && (
                  <div className="rounded-lg border-l-4 border-[var(--success)] bg-[var(--success)]/10 p-4">
                    <h4 className="font-bold text-[var(--text-primary)]">
                      Linux Notes
                    </h4>
                    <p className="mt-2 text-sm text-[var(--text-secondary)]">
                      No additional drivers required! OpenSoundDeck automatically creates 
                      virtual audio devices via PulseAudio or PipeWire.
                    </p>
                  </div>
                )}
              </div>
            )}

            {/* Alternative Downloads */}
            <div className="mt-12 border-t border-[var(--border)] pt-8">
              <h3 className="text-lg font-bold text-[var(--text-primary)]">
                Alternative Downloads
              </h3>
              <div className="mt-4 flex flex-wrap gap-4">
                <Link
                  href="https://github.com/kkrugley/OpenSoundDeck/releases"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="inline-flex items-center gap-2 rounded-md border border-[var(--border)] px-4 py-2 text-sm font-medium text-[var(--text-primary)] hover:bg-[var(--code-bg)]"
                >
                  All Releases
                  <ArrowRight className="h-3 w-3" />
                </Link>
                <Link
                  href="/docs"
                  className="inline-flex items-center gap-2 rounded-md border border-[var(--border)] px-4 py-2 text-sm font-medium text-[var(--text-primary)] hover:bg-[var(--code-bg)]"
                >
                  Build from Source
                  <ArrowRight className="h-3 w-3" />
                </Link>
              </div>
            </div>
          </div>
        </section>
      </main>
      
      <Footer currentLang={currentLang} onLanguageChange={setCurrentLang} />
    </div>
  );
}
