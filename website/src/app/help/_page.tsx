'use client';

import { useEffect, useState } from 'react';
import { useTranslation } from 'react-i18next';
import { Header } from '@/components/Header';
import { Footer } from '@/components/Footer';
import { EncryptedContactForm } from '@/components/EncryptedContactForm';
import { Question, BookOpen, GithubLogo, ArrowRight } from '@phosphor-icons/react';
import Link from 'next/link';
import { GITHUB_URL } from '@/lib/constants';

export default function HelpPage() {
  const { i18n, t } = useTranslation('translation');
  const [currentLang, setCurrentLang] = useState('en');
  const [mounted, setMounted] = useState(false);

  useEffect(() => {
    setMounted(true);
    setCurrentLang(i18n.language || 'en');
  }, [i18n.language]);

  const handleLanguageChange = (lang: string) => {
    i18n.changeLanguage(lang);
    setCurrentLang(lang);
    localStorage.setItem('i18nextLng', lang);
  };

  if (!mounted) {
    return (
      <div className="flex min-h-screen flex-col">
        <div className="flex-1 flex items-center justify-center">
          <div className="text-[var(--text-secondary)]">Loading...</div>
        </div>
      </div>
    );
  }

  return (
    <div className="flex min-h-screen flex-col">
      <Header currentLang={currentLang} onLanguageChange={handleLanguageChange} />
      
      <main className="flex-1">
        {/* Page Header */}
        <section className="w-full border-b border-[var(--border)] py-16">
          <div className="mx-auto max-w-4xl px-4 text-center sm:px-6 lg:px-8">
            <h1 className="text-3xl font-bold text-[var(--text-primary)] sm:text-4xl">
              {t('help.title')}
            </h1>
            <p className="mx-auto mt-4 max-w-2xl text-[var(--text-secondary)]">
              {t('help.description')}
            </p>
          </div>
        </section>

        {/* Quick Links */}
        <section className="w-full border-b border-[var(--border)] py-12">
          <div className="mx-auto max-w-4xl px-4 sm:px-6 lg:px-8">
            <div className="grid gap-6 sm:grid-cols-3">
              <Link
                href="/docs"
                className="group flex flex-col items-center rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6 text-center transition-all hover:border-[var(--text-primary)]"
              >
                <BookOpen className="h-8 w-8 text-[var(--text-primary)]" weight="bold" />
                <h3 className="mt-4 font-bold text-[var(--text-primary)]">{t('help.cards.docs.title')}</h3>
                <p className="mt-2 text-sm text-[var(--text-secondary)]">
                  {t('help.cards.docs.description')}
                </p>
                <span className="mt-4 inline-flex items-center gap-1 text-sm font-medium text-[var(--text-primary)] opacity-0 transition-opacity group-hover:opacity-100">
                  {t('help.cards.docs.action')} <ArrowRight className="h-3 w-3" />
                </span>
              </Link>

              <Link
                href={`${GITHUB_URL}/issues`}
                target="_blank"
                rel="noopener noreferrer"
                className="group flex flex-col items-center rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6 text-center transition-all hover:border-[var(--text-primary)]"
              >
                <GithubLogo className="h-8 w-8 text-[var(--text-primary)]" weight="bold" />
                <h3 className="mt-4 font-bold text-[var(--text-primary)]">{t('help.cards.github.title')}</h3>
                <p className="mt-2 text-sm text-[var(--text-secondary)]">
                  {t('help.cards.github.description')}
                </p>
                <span className="mt-4 inline-flex items-center gap-1 text-sm font-medium text-[var(--text-primary)] opacity-0 transition-opacity group-hover:opacity-100">
                  {t('help.cards.github.action')} <ArrowRight className="h-3 w-3" />
                </span>
              </Link>

              <Link
                href="#contact"
                className="group flex flex-col items-center rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6 text-center transition-all hover:border-[var(--text-primary)]"
              >
                <Question className="h-8 w-8 text-[var(--text-primary)]" weight="bold" />
                <h3 className="mt-4 font-bold text-[var(--text-primary)]">{t('help.cards.contact.title')}</h3>
                <p className="mt-2 text-sm text-[var(--text-secondary)]">
                  {t('help.cards.contact.description')}
                </p>
                <span className="mt-4 inline-flex items-center gap-1 text-sm font-medium text-[var(--text-primary)] opacity-0 transition-opacity group-hover:opacity-100">
                  {t('help.cards.contact.action')} <ArrowRight className="h-3 w-3" />
                </span>
              </Link>
            </div>
          </div>
        </section>

        {/* Contact Form Section */}
        <section id="contact" className="w-full py-16 sm:py-20">
          <div className="mx-auto max-w-2xl px-4 sm:px-6 lg:px-8">
            <EncryptedContactForm />
          </div>
        </section>
      </main>
      
      <Footer currentLang={currentLang} onLanguageChange={handleLanguageChange} />
    </div>
  );
}
