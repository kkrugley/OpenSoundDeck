'use client';

import { useState } from 'react';
import { 
  GithubLogo, 
  CurrencyBtc, 
  PaypalLogo, 
  Coffee,
  Copy,
  Check,
  ArrowRight,
  Heart
} from '@phosphor-icons/react';
import { DONATION_METHODS } from '@/lib/constants';
import { cn } from '@/lib/utils';

const iconMap: Record<string, React.ElementType> = {
  GithubLogo,
  CurrencyBtc,
  PaypalLogo,
  Coffee,
};

export function DonationTabs() {
  const [activeTab, setActiveTab] = useState(DONATION_METHODS[0].id);
  const [copiedAddress, setCopiedAddress] = useState<string | null>(null);

  const activeMethod = DONATION_METHODS.find(m => m.id === activeTab) || DONATION_METHODS[0];

  const handleCopy = async (address: string, name: string) => {
    await navigator.clipboard.writeText(address);
    setCopiedAddress(name);
    setTimeout(() => setCopiedAddress(null), 2000);
  };

  return (
    <div className="w-full">
      {/* Header */}
      <div className="mb-8 text-center">
        <div className="mx-auto mb-4 flex h-16 w-16 items-center justify-center rounded-full bg-[var(--code-bg)]">
          <Heart className="h-8 w-8 text-[var(--success)]" weight="fill" />
        </div>
        <h2 className="text-2xl font-bold text-[var(--text-primary)] sm:text-3xl">
          Support OpenSoundDeck
        </h2>
        <p className="mx-auto mt-4 max-w-xl text-[var(--text-secondary)]">
          OpenSoundDeck is free and open-source. Your support helps us maintain 
          and improve the project. Choose a method below to contribute.
        </p>
      </div>

      {/* Vertical Tabs Layout */}
      <div className="flex flex-col gap-6 lg:flex-row">
        {/* Tab Buttons - Vertical on desktop */}
        <div className="flex flex-col gap-2 lg:w-48">
          {DONATION_METHODS.map((method) => {
            const Icon = iconMap[method.icon] || Heart;
            return (
              <button
                key={method.id}
                onClick={() => setActiveTab(method.id)}
                className={cn(
                  "flex items-center gap-3 rounded-md px-4 py-3 text-left transition-all",
                  activeTab === method.id
                    ? "bg-[var(--accent-dark)] text-[var(--accent-light)]"
                    : "bg-[var(--code-bg)] text-[var(--text-secondary)] hover:bg-[var(--border)] hover:text-[var(--text-primary)]"
                )}
              >
                <Icon className="h-5 w-5" weight="bold" />
                <span className="font-medium">{method.name}</span>
              </button>
            );
          })}
        </div>

        {/* Content Area */}
        <div className="flex-1 rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
          {/* Method Header */}
          <div className="mb-6 flex items-center gap-3">
            {(() => {
              const Icon = iconMap[activeMethod.icon] || Heart;
              return (
                <div 
                  className="flex h-12 w-12 items-center justify-center rounded-md"
                  style={{ backgroundColor: activeMethod.color + '20' }}
                >
                  <Icon 
                    className="h-6 w-6" 
                    weight="bold" 
                    style={{ color: activeMethod.color }}
                  />
                </div>
              );
            })()}
            <div>
              <h3 className="text-lg font-bold text-[var(--text-primary)]">
                {activeMethod.name}
              </h3>
              <p className="text-sm text-[var(--text-secondary)]">
                {activeMethod.description}
              </p>
            </div>
          </div>

          {/* Method Content */}
          <div className="space-y-4">
            {activeMethod.id === 'github' && (
              <div>
                <p className="mb-4 text-sm text-[var(--text-secondary)]">
                  Support us through GitHub Sponsors. You will get recognition 
                  on our repository and access to exclusive updates.
                </p>
                <a
                  href={activeMethod.url}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="inline-flex items-center gap-2 rounded-md bg-[#24292e] px-6 py-3 text-sm font-medium text-white hover:opacity-90 transition-opacity"
                >
                  <GithubLogo className="h-4 w-4" weight="bold" />
                  Sponsor on GitHub
                  <ArrowRight className="h-4 w-4" />
                </a>
              </div>
            )}

            {activeMethod.id === 'crypto' && activeMethod.wallets && (
              <div className="space-y-4">
                <p className="text-sm text-[var(--text-secondary)]">
                  Send cryptocurrency to any of the addresses below. Make sure to 
                  use the correct network.
                </p>
                {activeMethod.wallets.map((wallet) => (
                  <div 
                    key={wallet.name}
                    className="rounded-md border border-[var(--border)] bg-[var(--background)] p-4"
                  >
                    <div className="mb-2 flex items-center justify-between">
                      <span className="text-sm font-medium text-[var(--text-primary)]">
                        {wallet.name}
                      </span>
                      <span className="text-xs text-[var(--text-tertiary)]">
                        {wallet.network}
                      </span>
                    </div>
                    <div className="flex items-center gap-2">
                      <code className="flex-1 truncate rounded bg-[var(--code-bg)] px-3 py-2 text-xs text-[var(--text-secondary)]">
                        {wallet.address}
                      </code>
                      <button
                        onClick={() => handleCopy(wallet.address, wallet.name)}
                        className="flex-shrink-0 rounded-md border border-[var(--border)] p-2 text-[var(--text-tertiary)] hover:bg-[var(--border)] hover:text-[var(--text-primary)] transition-colors"
                      >
                        {copiedAddress === wallet.name ? (
                          <Check className="h-4 w-4 text-[var(--success)]" weight="bold" />
                        ) : (
                          <Copy className="h-4 w-4" weight="bold" />
                        )}
                      </button>
                    </div>
                  </div>
                ))}
              </div>
            )}

            {activeMethod.id === 'paypal' && (
              <div>
                <p className="mb-4 text-sm text-[var(--text-secondary)]">
                  Make a one-time or recurring donation through PayPal. 
                  Fast, secure, and easy.
                </p>
                <a
                  href={activeMethod.url}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="inline-flex items-center gap-2 rounded-md px-6 py-3 text-sm font-medium text-white hover:opacity-90 transition-opacity"
                  style={{ backgroundColor: activeMethod.color }}
                >
                  <PaypalLogo className="h-4 w-4" weight="bold" />
                  Donate with PayPal
                  <ArrowRight className="h-4 w-4" />
                </a>
              </div>
            )}

            {activeMethod.id === 'kofi' && (
              <div>
                <p className="mb-4 text-sm text-[var(--text-secondary)]">
                  Buy us a coffee on Ko-fi. Your support helps keep the project alive
                  and motivates us to add new features.
                </p>
                <a
                  href={activeMethod.url}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="inline-flex items-center gap-2 rounded-md px-6 py-3 text-sm font-medium text-white hover:opacity-90 transition-opacity"
                  style={{ backgroundColor: activeMethod.color }}
                >
                  <Coffee className="h-4 w-4" weight="fill" />
                  Buy us a coffee
                  <ArrowRight className="h-4 w-4" />
                </a>
              </div>
            )}
          </div>
        </div>
      </div>

      {/* Thank You Message */}
      <div className="mt-8 text-center">
        <p className="text-sm text-[var(--text-tertiary)]">
          Thank you for supporting OpenSoundDeck! Every contribution helps 
          us build a better soundboard for everyone.
        </p>
      </div>
    </div>
  );
}
