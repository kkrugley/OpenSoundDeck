import type { Metadata } from 'next';
import { JetBrains_Mono } from 'next/font/google';
import { I18nProvider } from '@/components/I18nProvider';
import './globals.css';

const jetbrainsMono = JetBrains_Mono({
  subsets: ['latin'],
  variable: '--font-mono',
  display: 'swap',
});

export const metadata: Metadata = {
  title: 'OpenSoundDeck - The Open Source Soundboard',
  description: 'Cross-platform, open-source soundboard for voice chats and streams. Inject audio directly into your microphone with global hotkeys.',
  keywords: ['soundboard', 'open source', 'voice chat', 'streaming', 'microphone', 'audio', 'hotkeys', 'cross-platform'],
  authors: [{ name: 'OpenSoundDeck Team' }],
  openGraph: {
    title: 'OpenSoundDeck - The Open Source Soundboard',
    description: 'Cross-platform, open-source soundboard for voice chats and streams.',
    type: 'website',
    url: 'https://opensounddeck.vercel.app',
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
