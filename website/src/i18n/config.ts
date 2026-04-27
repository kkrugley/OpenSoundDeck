export type Locale = 'en' | 'ru' | 'pl';

export const defaultLocale: Locale = 'en';
export const locales: Locale[] = ['en', 'ru', 'pl'];

export const localeNames: Record<Locale, string> = {
  en: 'English',
  ru: 'Русский',
  pl: 'Polski',
};

export const localeFlags: Record<Locale, string> = {
  en: '🇺🇸',
  ru: '🇷🇺',
  pl: '🇵🇱',
};
