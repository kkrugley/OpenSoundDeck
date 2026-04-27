import type { Metadata } from 'next';
import { buildOgUrl } from '@/lib/og';
import { SITE_URL } from '@/lib/constants';
import SupportContent from './_page';

const ogImage = buildOgUrl({
  title: 'Support OpenSoundDeck',
  description:
    'Help keep OpenSoundDeck free and open-source. Donate via GitHub Sponsors, crypto, PayPal, or Ko-fi.',
  label: 'Donate',
  tags: ['GitHub Sponsors', 'Cryptocurrency', 'Ko-fi'],
});

export const metadata: Metadata = {
  title: 'Support OpenSoundDeck',
  description:
    'Help keep OpenSoundDeck free and open-source. Donate via GitHub Sponsors, cryptocurrency, PayPal, or Ko-fi.',
  openGraph: {
    title: 'Support OpenSoundDeck',
    description:
      'Help keep OpenSoundDeck free and open-source. Donate via GitHub Sponsors, crypto, PayPal, or Ko-fi.',
    url: `${SITE_URL}/support`,
    siteName: 'OpenSoundDeck',
    type: 'website',
    locale: 'en_US',
    alternateLocale: ['ru_RU', 'pl_PL'],
    images: [{ url: ogImage, width: 1200, height: 630, alt: 'Support OpenSoundDeck', type: 'image/png' }],
  },
  twitter: {
    card: 'summary_large_image',
    title: 'Support OpenSoundDeck',
    description: 'Help keep OpenSoundDeck free. Donate via GitHub Sponsors, crypto, PayPal, or Ko-fi.',
    images: [ogImage],
  },
};

export default function SupportPage() {
  return <SupportContent />;
}
