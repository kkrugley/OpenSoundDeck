'use client';

import Link from 'next/link';
import { ArrowRight, Shield } from '@phosphor-icons/react';
import { useTranslation } from 'react-i18next';

export function Privacy() {
  const { t } = useTranslation('translation');

  return (
    <section className="w-full border-t border-[var(--border)] py-16 sm:py-20">
      <div className="mx-auto max-w-3xl px-4 text-center sm:px-6 lg:px-8">
        {/* Icon */}
        <div className="mx-auto mb-6 flex h-16 w-16 items-center justify-center rounded-full bg-[var(--code-bg)]">
          <Shield className="h-8 w-8 text-[var(--text-primary)]" weight="bold" />
        </div>

        {/* Heading */}
        <h2 className="text-2xl font-bold text-[var(--text-primary)] sm:text-3xl">
          {t('privacy.title')}
        </h2>

        {/* Description */}
        <p className="mx-auto mt-4 max-w-xl text-[var(--text-secondary)]">
          {t('privacy.description')}
        </p>

        {/* Privacy Link */}
        <Link
          href="/legal"
          className="mt-6 inline-flex items-center gap-1 text-sm font-medium text-[var(--text-primary)] underline hover:no-underline"
        >
          {t('privacy.learnMore')}
          <ArrowRight className="h-3 w-3" />
        </Link>
      </div>
    </section>
  );
}
