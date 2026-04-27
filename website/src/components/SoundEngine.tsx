'use client';

import Link from 'next/link';
import { ArrowRight, SpeakerHigh, Cpu, Waveform, Sliders } from '@phosphor-icons/react';
import { useTranslation } from 'react-i18next';

export function SoundEngine() {
  const { t } = useTranslation('translation');

  const engineFeatures = [
    { icon: Waveform, label: t('engine.features.miniaudio') },
    { icon: Cpu, label: t('engine.features.cpp20') },
    { icon: SpeakerHigh, label: t('engine.features.lowLatency') },
    { icon: Sliders, label: t('engine.features.mixing') },
  ];

  return (
    <section className="w-full border-t border-[var(--border)] py-16 sm:py-20">
      <div className="mx-auto max-w-4xl px-4 text-center sm:px-6 lg:px-8">
        {/* Heading */}
        <h2 className="text-2xl font-bold text-[var(--text-primary)] sm:text-3xl">
          {t('engine.title')}
        </h2>

        {/* Description */}
        <p className="mx-auto mt-4 max-w-2xl text-[var(--text-secondary)]">
          {t('engine.description')}
        </p>

        {/* Engine Feature Logos */}
        <div className="mt-8 flex flex-wrap items-center justify-center gap-8">
          {engineFeatures.map((feature, index) => {
            const Icon = feature.icon;
            return (
              <div
                key={index}
                className="flex flex-col items-center gap-2 text-[var(--text-tertiary)] transition-colors hover:text-[var(--text-secondary)]"
              >
                <Icon className="h-8 w-8" weight="bold" />
                <span className="text-xs font-medium">{feature.label}</span>
              </div>
            );
          })}
        </div>

        {/* CTA */}
        <Link
          href="/docs"
          className="mt-8 inline-flex items-center gap-1 rounded-md border border-[var(--border)] px-6 py-3 text-sm font-medium text-[var(--text-primary)] hover:bg-[var(--code-bg)] transition-colors"
        >
          {t('engine.learnMore')}
          <ArrowRight className="h-4 w-4" />
        </Link>
      </div>
    </section>
  );
}
