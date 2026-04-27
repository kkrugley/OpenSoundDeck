'use client';

import { useState } from 'react';
import Link from 'next/link';
import { useTranslation } from 'react-i18next';
import { GithubLogo, List, X, Download, CaretDown } from '@phosphor-icons/react';
import { GITHUB_URL, LANGUAGES } from '@/lib/constants';
import { cn } from '@/lib/utils';

interface HeaderProps {
  currentLang: string;
  onLanguageChange: (lang: string) => void;
}

export function Header({ currentLang, onLanguageChange }: HeaderProps) {
  const [isMenuOpen, setIsMenuOpen] = useState(false);
  const [isLangOpen, setIsLangOpen] = useState(false);
  const { t } = useTranslation('translation');

  const currentLanguage = LANGUAGES.find(l => l.code === currentLang) || LANGUAGES[0];

  const handleLangChange = (code: string) => {
    onLanguageChange(code);
    setIsLangOpen(false);
  };

  const navLinks = [
    { name: t('nav.github'), href: GITHUB_URL, external: true },
    { name: t('nav.docs'), href: '/docs', external: false },
    { name: t('nav.download'), href: '/download', external: false },
    { name: t('nav.help'), href: '/help', external: false },
    { name: t('nav.support'), href: '/support', external: false },
  ];

  return (
    <header className="sticky top-0 z-50 w-full border-b border-[var(--border)] bg-[var(--background)]/95 backdrop-blur supports-[backdrop-filter]:bg-[var(--background)]/80">
      <div className="mx-auto flex h-16 max-w-7xl items-center justify-between px-4 sm:px-6 lg:px-8">
        {/* Logo */}
        <Link 
          href="/" 
          className="flex items-center gap-2 text-lg font-bold text-[var(--text-primary)] hover:opacity-80 transition-opacity"
        >
          <span className="text-[var(--success)]">🔊</span>
          <span>OpenSoundDeck</span>
        </Link>

        {/* Desktop Navigation */}
        <nav className="hidden md:flex items-center gap-6">
          {navLinks.map((link) => (
            <Link
              key={link.name}
              href={link.href}
              target={link.external ? '_blank' : undefined}
              rel={link.external ? 'noopener noreferrer' : undefined}
              className={cn(
                "text-sm font-medium text-[var(--text-secondary)] hover:text-[var(--text-primary)] transition-colors",
                link.name === t('nav.github') && "flex items-center gap-1"
              )}
            >
              {link.name === t('nav.github') && (
                <GithubLogo className="h-4 w-4" weight="bold" />
              )}
              {link.name}
            </Link>
          ))}
        </nav>

        {/* Right Section: Language + Download */}
        <div className="hidden md:flex items-center gap-4">
          {/* Language Selector */}
          <div className="relative">
            <button
              onClick={() => setIsLangOpen(!isLangOpen)}
              className="flex items-center gap-1 text-sm font-medium text-[var(--text-secondary)] hover:text-[var(--text-primary)] transition-colors"
            >
              <span className="text-base">{currentLanguage.flag}</span>
              <span>{currentLanguage.code.toUpperCase()}</span>
              <CaretDown className={cn("h-3 w-3 transition-transform", isLangOpen && "rotate-180")} />
            </button>

            {isLangOpen && (
              <div className="absolute right-0 top-full mt-2 w-40 rounded-md border border-[var(--border)] bg-[var(--background)] shadow-lg">
                {LANGUAGES.map((lang) => (
                  <button
                    key={lang.code}
                    onClick={() => handleLangChange(lang.code)}
                    className={cn(
                      "flex w-full items-center gap-2 px-4 py-2 text-left text-sm transition-colors hover:bg-[var(--code-bg)]",
                      currentLang === lang.code && "font-medium text-[var(--text-primary)]"
                    )}
                  >
                    <span className="text-base">{lang.flag}</span>
                    <span>{lang.name}</span>
                  </button>
                ))}
              </div>
            )}
          </div>

          {/* Download Button */}
          <Link
            href="/download"
            className="flex items-center gap-2 rounded-md bg-[var(--accent-dark)] px-4 py-2 text-sm font-medium text-[var(--accent-light)] hover:opacity-90 transition-opacity"
          >
            <Download className="h-4 w-4" weight="bold" />
            {t('nav.download')}
          </Link>
        </div>

        {/* Mobile Menu Button */}
        <button
          onClick={() => setIsMenuOpen(!isMenuOpen)}
          className="md:hidden p-2 text-[var(--text-primary)]"
          aria-label="Toggle menu"
        >
          {isMenuOpen ? (
            <X className="h-6 w-6" weight="bold" />
          ) : (
            <List className="h-6 w-6" weight="bold" />
          )}
        </button>
      </div>

      {/* Mobile Menu */}
      {isMenuOpen && (
        <div className="md:hidden border-t border-[var(--border)] bg-[var(--background)]">
          <div className="space-y-1 px-4 py-4">
            {navLinks.map((link) => (
              <Link
                key={link.name}
                href={link.href}
                target={link.external ? '_blank' : undefined}
                rel={link.external ? 'noopener noreferrer' : undefined}
                onClick={() => setIsMenuOpen(false)}
                className="flex items-center gap-2 rounded-md px-3 py-2 text-base font-medium text-[var(--text-secondary)] hover:bg-[var(--code-bg)] hover:text-[var(--text-primary)]"
              >
                {link.name === t('nav.github') && <GithubLogo className="h-4 w-4" />}
                {link.name}
              </Link>
            ))}

            {/* Mobile Language Selector */}
            <div className="border-t border-[var(--border)] pt-4 mt-4">
              <p className="px-3 text-xs font-medium text-[var(--text-tertiary)] uppercase tracking-wider mb-2">
                Language
              </p>
              {LANGUAGES.map((lang) => (
                <button
                  key={lang.code}
                  onClick={() => handleLangChange(lang.code)}
                  className={cn(
                    "flex w-full items-center gap-2 rounded-md px-3 py-2 text-base transition-colors hover:bg-[var(--code-bg)]",
                    currentLang === lang.code 
                      ? "font-medium text-[var(--text-primary)]" 
                      : "text-[var(--text-secondary)]"
                  )}
                >
                  <span className="text-lg">{lang.flag}</span>
                  <span>{lang.name}</span>
                </button>
              ))}
            </div>

            {/* Mobile Download Button */}
            <Link
              href="/download"
              onClick={() => setIsMenuOpen(false)}
              className="flex items-center justify-center gap-2 rounded-md bg-[var(--accent-dark)] px-4 py-3 text-base font-medium text-[var(--accent-light)] hover:opacity-90 transition-opacity mt-4"
            >
              <Download className="h-5 w-5" weight="bold" />
              {t('nav.download')}
            </Link>
          </div>
        </div>
      )}
    </header>
  );
}
