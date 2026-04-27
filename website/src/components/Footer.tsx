'use client';

import Link from 'next/link';
import { useTranslation } from 'react-i18next';
import { GithubLogo } from '@phosphor-icons/react';
import { GITHUB_URL, LANGUAGES } from '@/lib/constants';
import { cn } from '@/lib/utils';

interface FooterProps {
  currentLang: string;
  onLanguageChange: (lang: string) => void;
}

export function Footer({ currentLang, onLanguageChange }: FooterProps) {
  const { t } = useTranslation('translation');

  const handleLangChange = (code: string) => {
    onLanguageChange(code);
  };

  return (
    <footer className="w-full border-t border-[var(--border)] bg-[var(--background)]">
      <div className="mx-auto max-w-7xl px-4 py-12 sm:px-6 lg:px-8">
        {/* Main Footer Grid */}
        <div className="grid grid-cols-2 gap-8 md:grid-cols-5">
          {/* Brand Column */}
          <div className="col-span-2 md:col-span-1">
            <Link 
              href="/" 
              className="flex items-center gap-2 text-lg font-bold text-[var(--text-primary)] hover:opacity-80 transition-opacity"
            >
              <span className="text-[var(--success)]">🔊</span>
              <span>OpenSoundDeck</span>
            </Link>
            <p className="mt-4 text-sm text-[var(--text-secondary)]">
              {t('footer.description')}
            </p>
          </div>

          {/* Social Links */}
          <div>
            <h3 className="text-sm font-semibold text-[var(--text-primary)] uppercase tracking-wider">
              {t('footer.social')}
            </h3>
            <ul className="mt-4 space-y-3">
              <li>
                <Link
                  href={GITHUB_URL}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="flex items-center gap-2 text-sm text-[var(--text-secondary)] hover:text-[var(--text-primary)] transition-colors"
                >
                  <GithubLogo className="h-4 w-4" weight="bold" />
                  GitHub
                </Link>
              </li>
            </ul>
          </div>

          {/* Documentation Links */}
          <div>
            <h3 className="text-sm font-semibold text-[var(--text-primary)] uppercase tracking-wider">
              {t('footer.docs')}
            </h3>
            <ul className="mt-4 space-y-3">
              <li>
                <Link
                  href="/docs"
                  className="text-sm text-[var(--text-secondary)] hover:text-[var(--text-primary)] transition-colors"
                >
                  {t('footer.docs')}
                </Link>
              </li>
              <li>
                <Link
                  href={`${GITHUB_URL}/releases`}
                  className="text-sm text-[var(--text-secondary)] hover:text-[var(--text-primary)] transition-colors"
                >
                  Changelog
                </Link>
              </li>
            </ul>
          </div>

          {/* Legal Links */}
          <div>
            <h3 className="text-sm font-semibold text-[var(--text-primary)] uppercase tracking-wider">
              {t('footer.legal')}
            </h3>
            <ul className="mt-4 space-y-3">
              <li>
                <Link
                  href="/legal"
                  className="text-sm text-[var(--text-secondary)] hover:text-[var(--text-primary)] transition-colors"
                >
                  {t('footer.legal')}
                </Link>
              </li>
            </ul>
          </div>

          {/* Language Selector */}
          <div>
            <h3 className="text-sm font-semibold text-[var(--text-primary)] uppercase tracking-wider">
              {t('footer.language')}
            </h3>
            <div className="mt-4 space-y-2">
              {LANGUAGES.map((lang) => (
                <button
                  key={lang.code}
                  onClick={() => handleLangChange(lang.code)}
                  className={cn(
                    "flex w-full items-center gap-2 text-sm transition-colors",
                    currentLang === lang.code 
                      ? "font-medium text-[var(--text-primary)]" 
                      : "text-[var(--text-secondary)] hover:text-[var(--text-primary)]"
                  )}
                >
                  <span className="text-base">{lang.flag}</span>
                  <span>{lang.name}</span>
                </button>
              ))}
            </div>
          </div>
        </div>

        {/* Bottom Bar */}
        <div className="mt-12 flex flex-col items-center justify-between gap-4 border-t border-[var(--border)] pt-8 sm:flex-row">
          <p className="text-sm text-[var(--text-tertiary)]">
            {t('footer.copyright', { year: new Date().getFullYear() })}
          </p>
          <p className="text-sm text-[var(--text-tertiary)]">
            {t('footer.builtWith')}
          </p>
        </div>
      </div>
    </footer>
  );
}
