'use client';

import { useEffect, useState } from 'react';
import { useTranslation } from 'react-i18next';
import { Header } from '@/components/Header';
import { Footer } from '@/components/Footer';
import { DonationTabs } from '@/components/DonationTabs';

export default function SupportPage() {
  const { i18n } = useTranslation();
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
        <section className="w-full py-16 sm:py-20">
          <div className="mx-auto max-w-4xl px-4 sm:px-6 lg:px-8">
            <DonationTabs />
          </div>
        </section>
      </main>
      
      <Footer currentLang={currentLang} onLanguageChange={handleLanguageChange} />
    </div>
  );
}
