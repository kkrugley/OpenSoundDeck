'use client';

import { Plus } from '@phosphor-icons/react';
import { useTranslation } from 'react-i18next';
import { cn } from '@/lib/utils';

interface FeaturesProps {
  className?: string;
}

export function Features({ className }: FeaturesProps) {
  const { t } = useTranslation('translation');

  const featuresList = [
    {
      title: t('features.list.micInjection.title'),
      description: t('features.list.micInjection.description'),
    },
    {
      title: t('features.list.globalHotkeys.title'),
      description: t('features.list.globalHotkeys.description'),
    },
    {
      title: t('features.list.crossPlatform.title'),
      description: t('features.list.crossPlatform.description'),
    },
    {
      title: t('features.list.soundMixing.title'),
      description: t('features.list.soundMixing.description'),
    },
    {
      title: t('features.list.simpleUI.title'),
      description: t('features.list.simpleUI.description'),
    },
    {
      title: t('features.list.openSource.title'),
      description: t('features.list.openSource.description'),
    },
    {
      title: t('features.list.lowLatency.title'),
      description: t('features.list.lowLatency.description'),
    },
  ];

  return (
    <section className={cn("w-full py-16 sm:py-20", className)}>
      <div className="mx-auto max-w-4xl px-4 sm:px-6 lg:px-8">
        {/* Section Header */}
        <div className="mb-12">
          <h2 className="text-2xl font-bold text-[var(--text-primary)] sm:text-3xl">
            {t('features.title')}
          </h2>
          <p className="mt-4 text-[var(--text-secondary)]">
            {t('features.description')}
          </p>
        </div>

        {/* Features List */}
        <div className="space-y-6">
          {featuresList.map((feature, index) => (
            <div
              key={index}
              className="group flex items-start gap-4 rounded-lg border border-transparent p-4 transition-colors hover:border-[var(--border)] hover:bg-[var(--code-bg)]"
            >
              <div className="flex-shrink-0">
                <div className="flex h-6 w-6 items-center justify-center rounded bg-[var(--accent-dark)] text-[var(--accent-light)]">
                  <Plus className="h-4 w-4" weight="bold" />
                </div>
              </div>
              <div>
                <h3 className="font-bold text-[var(--text-primary)]">
                  {feature.title}
                </h3>
                <p className="mt-1 text-sm text-[var(--text-secondary)]">
                  {feature.description}
                </p>
              </div>
            </div>
          ))}
        </div>
      </div>
    </section>
  );
}
