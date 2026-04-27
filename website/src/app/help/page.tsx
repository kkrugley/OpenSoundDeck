import type { Metadata } from 'next';
import { buildOgUrl } from '@/lib/og';
import { SITE_URL } from '@/lib/constants';
import HelpContent from './_page';

const ogImage = buildOgUrl({
  title: 'Help & Support',
  description:
    'Get help with OpenSoundDeck. Browse documentation, open a GitHub issue, or contact us with PGP-encrypted messages.',
  label: 'Support',
  tags: ['Documentation', 'GitHub Issues', 'PGP Encrypted'],
});

export const metadata: Metadata = {
  title: 'Help & Support — OpenSoundDeck',
  description:
    'Need help with OpenSoundDeck? Browse documentation, report bugs on GitHub, or send us a PGP-encrypted message.',
  openGraph: {
    title: 'Help & Support — OpenSoundDeck',
    description:
      'Get help with OpenSoundDeck. Browse documentation, open a GitHub issue, or contact us with PGP-encrypted messages.',
    url: `${SITE_URL}/help`,
    siteName: 'OpenSoundDeck',
    type: 'website',
    locale: 'en_US',
    alternateLocale: ['ru_RU', 'pl_PL'],
    images: [{ url: ogImage, width: 1200, height: 630, alt: 'Help & Support — OpenSoundDeck', type: 'image/png' }],
  },
  twitter: {
    card: 'summary_large_image',
    title: 'Help & Support — OpenSoundDeck',
    description: 'Documentation, GitHub issues, and PGP-encrypted contact.',
    images: [ogImage],
  },
};

export default function HelpPage() {
  return <HelpContent />;
}
