import type { Metadata } from 'next';
import { buildOgUrl } from '@/lib/og';
import { SITE_URL } from '@/lib/constants';
import DownloadContent from './_page';

const ogImage = buildOgUrl({
  title: 'Download OpenSoundDeck',
  description: 'Free downloads for Linux, Windows, and macOS. AppImage, Flatpak, MSI, DMG — pick your format.',
  label: 'Download',
  tags: ['Linux', 'Windows', 'macOS'],
});

export const metadata: Metadata = {
  title: 'Download — OpenSoundDeck',
  description:
    'Download OpenSoundDeck for Linux, Windows, or macOS. Free, open-source, and always up to date.',
  openGraph: {
    title: 'Download OpenSoundDeck',
    description: 'Free downloads for Linux, Windows, and macOS. AppImage, Flatpak, MSI, DMG — pick your format.',
    url: `${SITE_URL}/download`,
    siteName: 'OpenSoundDeck',
    type: 'website',
    locale: 'en_US',
    alternateLocale: ['ru_RU', 'pl_PL'],
    images: [{ url: ogImage, width: 1200, height: 630, alt: 'Download OpenSoundDeck', type: 'image/png' }],
  },
  twitter: {
    card: 'summary_large_image',
    title: 'Download OpenSoundDeck',
    description: 'Free downloads for Linux, Windows, and macOS.',
    images: [ogImage],
  },
};

export default function DownloadPage() {
  return <DownloadContent />;
}
