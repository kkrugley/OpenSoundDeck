import type { Metadata } from 'next';
import { buildOgUrl } from '@/lib/og';
import { SITE_URL } from '@/lib/constants';
import LegalContent from './_page';

const ogImage = buildOgUrl({
  title: 'Privacy Policy & Terms',
  description:
    'OpenSoundDeck collects zero data. All audio processing is local. MIT-licensed, transparent, and open source.',
  label: 'Legal',
  tags: ['Privacy First', 'Zero Telemetry', 'MIT License'],
});

export const metadata: Metadata = {
  title: 'Legal — OpenSoundDeck',
  description:
    'Privacy policy, terms of service, and license information for OpenSoundDeck. We collect zero data — all processing is local.',
  openGraph: {
    title: 'Privacy Policy & Terms — OpenSoundDeck',
    description:
      'OpenSoundDeck collects zero data. All audio processing is local. MIT-licensed, transparent, and open source.',
    url: `${SITE_URL}/legal`,
    siteName: 'OpenSoundDeck',
    type: 'website',
    locale: 'en_US',
    alternateLocale: ['ru_RU', 'pl_PL'],
    images: [{ url: ogImage, width: 1200, height: 630, alt: 'Legal — OpenSoundDeck', type: 'image/png' }],
  },
  twitter: {
    card: 'summary_large_image',
    title: 'Legal — OpenSoundDeck',
    description: 'Privacy policy, terms of service, and MIT license details.',
    images: [ogImage],
  },
};

export default function LegalPage() {
  return <LegalContent />;
}
