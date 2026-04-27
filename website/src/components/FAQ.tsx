'use client';

import { useState } from 'react';
import { Plus, Minus } from '@phosphor-icons/react';
import { useTranslation } from 'react-i18next';
import { cn } from '@/lib/utils';

export function FAQ() {
  const [openIndex, setOpenIndex] = useState<number | null>(0);
  const { t } = useTranslation('translation');

  const faqItems = [
    { question: t('faq.items.whatIs.question'), answer: t('faq.items.whatIs.answer') },
    { question: t('faq.items.howToUse.question'), answer: t('faq.items.howToUse.answer') },
    { question: t('faq.items.extraSoftware.question'), answer: t('faq.items.extraSoftware.answer') },
    { question: t('faq.items.free.question'), answer: t('faq.items.free.answer') },
    { question: t('faq.items.discord.question'), answer: t('faq.items.discord.answer') },
    { question: t('faq.items.formats.question'), answer: t('faq.items.formats.answer') },
    { question: t('faq.items.dataPrivate.question'), answer: t('faq.items.dataPrivate.answer') },
    { question: t('faq.items.contribute.question'), answer: t('faq.items.contribute.answer') },
  ];

  const toggleFAQ = (index: number) => {
    setOpenIndex(openIndex === index ? null : index);
  };

  return (
    <section className="w-full border-t border-[var(--border)] py-16 sm:py-20">
      <div className="mx-auto max-w-3xl px-4 sm:px-6 lg:px-8">
        {/* Section Header */}
        <h2 className="text-2xl font-bold text-[var(--text-primary)] sm:text-3xl">
          {t('faq.title')}
        </h2>

        {/* FAQ Items */}
        <div className="mt-8 space-y-0">
          {faqItems.map((item, index) => (
            <div
              key={index}
              className="border-b border-[var(--border)] last:border-b-0"
            >
              <button
                onClick={() => toggleFAQ(index)}
                className="flex w-full items-center justify-between py-5 text-left transition-colors hover:opacity-80"
                aria-expanded={openIndex === index}
              >
                <span className="pr-4 text-base font-bold text-[var(--text-primary)]">
                  {item.question}
                </span>
                <span className="flex-shrink-0 text-[var(--text-primary)]">
                  {openIndex === index ? (
                    <Minus className="h-5 w-5" weight="bold" />
                  ) : (
                    <Plus className="h-5 w-5" weight="bold" />
                  )}
                </span>
              </button>
              
              {/* Answer */}
              <div
                className={cn(
                  "overflow-hidden transition-all duration-200",
                  openIndex === index ? "max-h-96 pb-5" : "max-h-0"
                )}
              >
                <p className="text-[var(--text-secondary)]">
                  {item.answer}
                </p>
              </div>
            </div>
          ))}
        </div>
      </div>
    </section>
  );
}
