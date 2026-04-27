'use client';

import { useEffect, useState } from 'react';
import { useTranslation } from 'react-i18next';
import { Header } from '@/components/Header';
import { Footer } from '@/components/Footer';
import { Hero } from '@/components/Hero';
import { Features } from '@/components/Features';
import { Statistics } from '@/components/Statistics';
import { Privacy } from '@/components/Privacy';
import { FAQ } from '@/components/FAQ';
import { SoundEngine } from '@/components/SoundEngine';

export default function Home() {
  const { i18n } = useTranslation();
  const [currentLang, setCurrentLang] = useState('en');
  const [mounted, setMounted] = useState(false);

  useEffect(() => {
    setMounted(true);
    // Set initial language from i18n
    setCurrentLang(i18n.language || 'en');
  }, [i18n.language]);

  const handleLanguageChange = (lang: string) => {
    i18n.changeLanguage(lang);
    setCurrentLang(lang);
    // Store in localStorage for persistence
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
        <Hero />
        <Features />
        <Statistics />
        <Privacy />
        <FAQ />
        <SoundEngine />
      </main>
      
      <Footer currentLang={currentLang} onLanguageChange={handleLanguageChange} />
    </div>
  );
}
