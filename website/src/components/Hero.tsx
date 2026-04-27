'use client';

import { useState } from 'react';
import Link from 'next/link';
import { useTranslation } from 'react-i18next';
import { Copy, Check, Download, ArrowRight } from '@phosphor-icons/react';
import { INSTALL_COMMANDS } from '@/lib/constants';
import { cn } from '@/lib/utils';

const TABS = [
  { id: 'git', label: 'git', disabled: false },
  { id: 'curl', label: 'curl', disabled: true },
  { id: 'brew', label: 'brew', disabled: true },
  { id: 'choco', label: 'choco', disabled: true },
] as const;

export function Hero() {
  const [activeTab, setActiveTab] = useState<'curl' | 'git' | 'brew' | 'choco'>('git');
  const [copied, setCopied] = useState(false);
  const { t } = useTranslation('translation');

  const handleCopy = async () => {
    const command = activeTab === 'brew' 
      ? 'brew install opensounddeck  # Coming soon'
      : activeTab === 'choco'
      ? 'choco install opensounddeck  # Coming soon'
      : INSTALL_COMMANDS[activeTab as keyof typeof INSTALL_COMMANDS];
    await navigator.clipboard.writeText(command);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const getCommand = () => {
    if (activeTab === 'brew') return 'brew install opensounddeck  # Coming soon';
    if (activeTab === 'choco') return 'choco install opensounddeck  # Coming soon';
    return INSTALL_COMMANDS[activeTab as keyof typeof INSTALL_COMMANDS];
  };

  const activeTabData = TABS.find(tab => tab.id === activeTab);

  return (
    <section className="w-full py-16 sm:py-24 lg:py-32">
      <div className="mx-auto max-w-4xl px-4 sm:px-6 lg:px-8">
        {/* Main Heading */}
        <h1 className="text-center text-4xl font-bold leading-tight text-[var(--text-primary)] sm:text-5xl lg:text-6xl">
          {t('hero.title')}
        </h1>

        {/* Tagline */}
        <p className="mx-auto mt-6 max-w-2xl text-center text-lg text-[var(--text-secondary)] sm:text-xl">
          {t('hero.tagline')}
        </p>

        {/* Installation Section */}
        <div className="mx-auto mt-10 max-w-2xl">
          {/* Tabs */}
          <div className="flex flex-wrap gap-2 border-b border-[var(--border)]">
            {TABS.map((tab) => (
              <button
                key={tab.id}
                onClick={() => setActiveTab(tab.id)}
                disabled={tab.disabled}
                className={cn(
                  "px-4 py-2 text-sm font-medium transition-colors relative",
                  activeTab === tab.id
                    ? "border-b-2 border-[var(--text-primary)] text-[var(--text-primary)]"
                    : "text-[var(--text-tertiary)] hover:text-[var(--text-secondary)]",
                  tab.disabled && "opacity-50 cursor-not-allowed hover:text-[var(--text-tertiary)]"
                )}
              >
                {tab.label}
                {tab.disabled && (
                  <span className="ml-1 text-xs text-[var(--text-tertiary)]">*</span>
                )}
              </button>
            ))}
          </div>

          {/* Code Block */}
          <div className="relative mt-0 rounded-b-md rounded-tr-md border border-[var(--border)] bg-[var(--code-bg)] p-4">
            <code className={cn(
              "block overflow-x-auto pr-12 text-sm font-mono",
              activeTabData?.disabled ? "text-[var(--text-tertiary)]" : "text-[var(--text-primary)]"
            )}>
              {getCommand()}
            </code>
            <button
              onClick={handleCopy}
              className="absolute right-3 top-1/2 -translate-y-1/2 rounded-md p-2 text-[var(--text-tertiary)] hover:bg-[var(--border)] hover:text-[var(--text-primary)] transition-colors"
              aria-label={copied ? 'Copied' : 'Copy to clipboard'}
            >
              {copied ? (
                <Check className="h-4 w-4 text-[var(--success)]" weight="bold" />
              ) : (
                <Copy className="h-4 w-4" weight="bold" />
              )}
            </button>
          </div>

          {/* Coming soon note */}
          {activeTabData?.disabled && (
            <p className="mt-2 text-xs text-[var(--text-tertiary)]">
              * {t('hero.comingSoonNote')}
            </p>
          )}
        </div>

        {/* CTA Buttons */}
        <div className="mt-10 flex flex-col items-center justify-center gap-4 sm:flex-row">
          <Link
            href="/download"
            className="flex items-center gap-2 rounded-md bg-[var(--accent-dark)] px-6 py-3 text-base font-medium text-[var(--accent-light)] hover:opacity-90 transition-opacity"
          >
            <Download className="h-5 w-5" weight="bold" />
            {t('hero.download')}
          </Link>
          <Link
            href="/docs"
            className="flex items-center gap-2 rounded-md border border-[var(--border)] bg-[var(--accent-light)] px-6 py-3 text-base font-medium text-[var(--text-primary)] hover:bg-[var(--code-bg)] transition-colors"
          >
            {t('hero.readDocs')}
            <ArrowRight className="h-4 w-4" />
          </Link>
        </div>
      </div>
    </section>
  );
}
