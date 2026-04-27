import type { Metadata } from 'next';
import { JetBrains_Mono } from 'next/font/google';
import { I18nProvider } from '@/components/I18nProvider';
import { SITE_URL } from '@/lib/constants';
import { buildOgUrl } from '@/lib/og';
import './globals.css';

const jetbrainsMono = JetBrains_Mono({
  subsets: ['latin'],
  variable: '--font-mono',
  display: 'swap',
});

const defaultOgImage = buildOgUrl({
  title: 'The Open Source Soundboard',
  description: 'Inject audio directly into your microphone with global hotkeys. Cross-platform, free.',
  tags: ['Open Source', 'Cross-Platform', 'MIT License'],
});

export const metadata: Metadata = {
  metadataBase: new URL(SITE_URL),
  title: {
    default: 'OpenSoundDeck — The Open Source Soundboard',
    template: '%s — OpenSoundDeck',
  },
  description:
    'Cross-platform, open-source soundboard for voice chats and streams. Inject audio directly into your microphone with global hotkeys.',
  keywords: [
    'soundboard', 'open source', 'voice chat', 'streaming', 'microphone',
    'audio', 'hotkeys', 'cross-platform', 'discord', 'linux', 'windows', 'macos', 'free',
  ],
  authors: [{ name: 'OpenSoundDeck Contributors' }],
  creator: 'OpenSoundDeck',
  publisher: 'OpenSoundDeck',
  robots: { index: true, follow: true },
  openGraph: {
    title: 'OpenSoundDeck — The Open Source Soundboard',
    description:
      'Cross-platform, open-source soundboard for voice chats and streams. Inject audio directly into your microphone with global hotkeys.',
    url: SITE_URL,
    siteName: 'OpenSoundDeck',
    type: 'website',
    locale: 'en_US',
    alternateLocale: ['ru_RU', 'pl_PL'],
    images: [
      {
        url: defaultOgImage,
        width: 1200,
        height: 630,
        alt: 'OpenSoundDeck — The Open Source Soundboard',
        type: 'image/png',
      },
    ],
  },
  twitter: {
    card: 'summary_large_image',
    title: 'OpenSoundDeck — The Open Source Soundboard',
    description:
      'Cross-platform, open-source soundboard for voice chats and streams.',
    images: [defaultOgImage],
  },
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en" className={jetbrainsMono.variable}>
      <body className="antialiased">
        <I18nProvider>
          {children}
        </I18nProvider>
      </body>
    </html>
  );
}
