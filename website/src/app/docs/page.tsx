import type { Metadata } from 'next';
import { buildOgUrl } from '@/lib/og';
import { SITE_URL } from '@/lib/constants';
import DocsContent from './_page';

const ogImage = buildOgUrl({
  title: 'Documentation',
  description:
    'Getting started guide, installation, configuration, and troubleshooting for OpenSoundDeck.',
  label: 'Docs',
  tags: ['Getting Started', 'Installation', 'Configuration'],
});

export const metadata: Metadata = {
  title: 'Documentation — OpenSoundDeck',
  description:
    'Getting started guide, installation instructions, configuration tips, and troubleshooting for OpenSoundDeck.',
  openGraph: {
    title: 'Documentation — OpenSoundDeck',
    description:
      'Getting started guide, installation, configuration, and troubleshooting for OpenSoundDeck.',
    url: `${SITE_URL}/docs`,
    siteName: 'OpenSoundDeck',
    type: 'website',
    locale: 'en_US',
    alternateLocale: ['ru_RU', 'pl_PL'],
    images: [{ url: ogImage, width: 1200, height: 630, alt: 'OpenSoundDeck Documentation', type: 'image/png' }],
  },
  twitter: {
    card: 'summary_large_image',
    title: 'Documentation — OpenSoundDeck',
    description: 'Getting started guide, installation, configuration, and troubleshooting.',
    images: [ogImage],
  },
};

export default function DocsPage() {
  return <DocsContent />;
}
