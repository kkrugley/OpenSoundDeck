import type { Metadata } from 'next';
import { buildOgUrl } from '@/lib/og';
import { SITE_URL } from '@/lib/constants';
import HomeContent from './_home';

const ogImage = buildOgUrl({
  title: 'The Open Source Soundboard',
  description:
    'Inject audio directly into your microphone with global hotkeys. Cross-platform, free, and built for streamers and gamers.',
  tags: ['Open Source', 'Cross-Platform', 'C++20 Native', 'MIT License'],
});

export const metadata: Metadata = {
  title: 'OpenSoundDeck — The Open Source Soundboard',
  description:
    'Inject audio directly into your microphone with global hotkeys. Cross-platform, free, and built for streamers and gamers.',
  keywords: [
    'soundboard', 'open source', 'voice chat', 'streaming', 'microphone',
    'audio', 'hotkeys', 'cross-platform', 'discord', 'linux', 'windows', 'macos',
  ],
  openGraph: {
    title: 'OpenSoundDeck — The Open Source Soundboard',
    description:
      'Inject audio directly into your microphone with global hotkeys. Cross-platform, free, and built for streamers and gamers.',
    url: SITE_URL,
    siteName: 'OpenSoundDeck',
    type: 'website',
    locale: 'en_US',
    alternateLocale: ['ru_RU', 'pl_PL'],
    images: [{ url: ogImage, width: 1200, height: 630, alt: 'OpenSoundDeck — The Open Source Soundboard', type: 'image/png' }],
  },
  twitter: {
    card: 'summary_large_image',
    title: 'OpenSoundDeck — The Open Source Soundboard',
    description: 'Inject audio directly into your microphone with global hotkeys. Cross-platform, free.',
    images: [ogImage],
  },
};

export default function HomePage() {
  return <HomeContent />;
}
