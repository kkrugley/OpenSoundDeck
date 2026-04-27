'use client';

import { useState } from 'react';
import { ArrowRight, CheckCircle } from '@phosphor-icons/react';

export function Newsletter() {
  const [email, setEmail] = useState('');
  const [status, setStatus] = useState<'idle' | 'success' | 'error'>('idle');

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (email) {
      // Simulate subscription
      setStatus('success');
      setEmail('');
      setTimeout(() => setStatus('idle'), 3000);
    }
  };

  return (
    <section className="w-full border-t border-[var(--border)] py-16 sm:py-20">
      <div className="mx-auto max-w-xl px-4 text-center sm:px-6 lg:px-8">
        {/* Heading */}
        <h2 className="text-2xl font-bold text-[var(--text-primary)] sm:text-3xl">
          Be the first to know
        </h2>

        {/* Description */}
        <p className="mt-4 text-[var(--text-secondary)]">
          Join our newsletter to get updates on new features, releases, and tips 
          for using OpenSoundDeck.
        </p>

        {/* Form */}
        <form onSubmit={handleSubmit} className="mt-8">
          <div className="flex flex-col gap-3 sm:flex-row">
            <input
              type="email"
              value={email}
              onChange={(e) => setEmail(e.target.value)}
              placeholder="Enter your email"
              required
              className="flex-1 rounded-md border border-[var(--border)] bg-[var(--input-bg)] px-4 py-3 text-sm text-[var(--text-primary)] placeholder:text-[var(--text-tertiary)] focus:border-[var(--text-primary)] focus:outline-none"
            />
            <button
              type="submit"
              className="flex items-center justify-center gap-2 rounded-md bg-[var(--accent-dark)] px-6 py-3 text-sm font-medium text-[var(--accent-light)] hover:opacity-90 transition-opacity"
            >
              Subscribe
              <ArrowRight className="h-4 w-4" />
            </button>
          </div>

          {/* Status Message */}
          {status === 'success' && (
            <div className="mt-4 flex items-center justify-center gap-2 text-sm text-[var(--success)]">
              <CheckCircle className="h-4 w-4" weight="bold" />
              <span>You have been subscribed!</span>
            </div>
          )}
        </form>
      </div>
    </section>
  );
}
