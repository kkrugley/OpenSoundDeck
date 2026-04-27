'use client';

import { useState, useRef } from 'react';
import { 
  Shield, 
  Copy, 
  Check, 
  Hourglass, 
  PaperPlaneRight,
  X,
  Key
} from '@phosphor-icons/react';
import * as openpgp from 'openpgp';
import { PUBLIC_GPG_KEY, WEB3FORMS_ENDPOINT } from '@/lib/constants';
import { cn } from '@/lib/utils';

export function EncryptedContactForm() {
  const [formData, setFormData] = useState({
    name: '',
    email: '',
    message: '',
  });
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [status, setStatus] = useState<'idle' | 'success' | 'error'>('idle');
  const [showKeyModal, setShowKeyModal] = useState(false);
  const [copied, setCopied] = useState(false);

  const handleChange = (e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
    setFormData(prev => ({
      ...prev,
      [e.target.name]: e.target.value,
    }));
  };

  const handleCopyKey = async () => {
    await navigator.clipboard.writeText(PUBLIC_GPG_KEY);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setIsSubmitting(true);
    setStatus('idle');

    try {
      // Create full message
      const fullMessage = `From: ${formData.name}
Email: ${formData.email}
Date: ${new Date().toLocaleString('en-US')}

Message:
${formData.message}`;

      // Encrypt the message
      const encrypted = await openpgp.encrypt({
        message: await openpgp.createMessage({ text: fullMessage }),
        encryptionKeys: await openpgp.readKey({ armoredKey: PUBLIC_GPG_KEY }),
        format: 'armored',
      });

      // Send encrypted message via Web3Forms
      const response = await fetch(WEB3FORMS_ENDPOINT, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          access_key: process.env.NEXT_PUBLIC_WEB3FORMS_KEY || '',
          subject: 'Encrypted Message @ OpenSoundDeck',
          from_name: 'Anonymous',
          from_email: 'noreply@opensounddeck.vercel.app',
          message: encrypted,
        }),
      });

      if (response.ok) {
        setStatus('success');
        setFormData({ name: '', email: '', message: '' });
      } else {
        setStatus('error');
      }
    } catch (error) {
      console.error('Encryption/Submission error:', error);
      setStatus('error');
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <div className="w-full">
      {/* Header */}
      <div className="mb-8 flex items-start gap-4">
        <div className="flex h-12 w-12 flex-shrink-0 items-center justify-center rounded-full bg-[var(--code-bg)]">
          <Shield className="h-6 w-6 text-[var(--success)]" weight="bold" />
        </div>
        <div>
          <h2 className="text-xl font-bold text-[var(--text-primary)]">
            Encrypted Contact
          </h2>
          <p className="mt-2 text-sm text-[var(--text-secondary)]">
            Send us an encrypted message using PGP. Your message will be encrypted 
            in your browser before being sent to our servers.
          </p>
          <button
            onClick={() => setShowKeyModal(true)}
            className="mt-2 inline-flex items-center gap-1 text-sm text-[var(--text-primary)] underline hover:no-underline"
          >
            <Key className="h-3 w-3" />
            View our public key
          </button>
        </div>
      </div>

      {/* Form */}
      <form onSubmit={handleSubmit} className="space-y-6">
        <div>
          <label 
            htmlFor="name" 
            className="block text-sm font-medium text-[var(--text-primary)]"
          >
            Name
          </label>
          <input
            type="text"
            id="name"
            name="name"
            value={formData.name}
            onChange={handleChange}
            required
            className="mt-1 block w-full rounded-md border border-[var(--border)] bg-[var(--input-bg)] px-4 py-3 text-sm text-[var(--text-primary)] placeholder:text-[var(--text-tertiary)] focus:border-[var(--text-primary)] focus:outline-none"
            placeholder="Your name"
          />
        </div>

        <div>
          <label 
            htmlFor="email" 
            className="block text-sm font-medium text-[var(--text-primary)]"
          >
            Email
          </label>
          <input
            type="email"
            id="email"
            name="email"
            value={formData.email}
            onChange={handleChange}
            required
            className="mt-1 block w-full rounded-md border border-[var(--border)] bg-[var(--input-bg)] px-4 py-3 text-sm text-[var(--text-primary)] placeholder:text-[var(--text-tertiary)] focus:border-[var(--text-primary)] focus:outline-none"
            placeholder="your@email.com"
          />
        </div>

        <div>
          <label 
            htmlFor="message" 
            className="block text-sm font-medium text-[var(--text-primary)]"
          >
            Message
          </label>
          <textarea
            id="message"
            name="message"
            value={formData.message}
            onChange={handleChange}
            required
            rows={5}
            className="mt-1 block w-full resize-none rounded-md border border-[var(--border)] bg-[var(--input-bg)] px-4 py-3 text-sm text-[var(--text-primary)] placeholder:text-[var(--text-tertiary)] focus:border-[var(--text-primary)] focus:outline-none"
            placeholder="Your message..."
          />
        </div>

        {/* Submit Button */}
        <button
          type="submit"
          disabled={isSubmitting}
          className={cn(
            "flex w-full items-center justify-center gap-2 rounded-md px-6 py-3 text-sm font-medium text-[var(--accent-light)] transition-all",
            isSubmitting 
              ? "bg-[var(--text-tertiary)] cursor-not-allowed" 
              : "bg-[var(--accent-dark)] hover:opacity-90"
          )}
        >
          {isSubmitting ? (
            <>
              <Hourglass className="h-4 w-4 animate-pulse" weight="bold" />
              <span>Encrypting message...</span>
            </>
          ) : (
            <>
              <Shield className="h-4 w-4" weight="bold" />
              <span>Send encrypted message</span>
            </>
          )}
        </button>

        {/* Status Messages */}
        {status === 'success' && (
          <div className="rounded-md border border-[var(--success)] bg-[var(--success)]/10 p-4 text-sm text-[var(--success)]">
            <div className="flex items-center gap-2">
              <Check className="h-4 w-4" weight="bold" />
              <span>Encrypted message sent successfully!</span>
            </div>
          </div>
        )}

        {status === 'error' && (
          <div className="rounded-md border border-red-500 bg-red-500/10 p-4 text-sm text-red-600">
            <div className="flex items-center gap-2">
              <X className="h-4 w-4" weight="bold" />
              <span>Failed to send message. Please try again.</span>
            </div>
          </div>
        )}
      </form>

      {/* Public Key Modal */}
      {showKeyModal && (
        <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/50 p-4">
          <div className="relative max-h-[80vh] w-full max-w-2xl overflow-hidden rounded-lg border border-[var(--border)] bg-[var(--background)] shadow-xl">
            {/* Modal Header */}
            <div className="flex items-center justify-between border-b border-[var(--border)] px-6 py-4">
              <div className="flex items-center gap-2">
                <Key className="h-5 w-5 text-[var(--success)]" weight="bold" />
                <h3 className="text-lg font-bold text-[var(--text-primary)]">
                  Public PGP Key
                </h3>
              </div>
              <button
                onClick={() => setShowKeyModal(false)}
                className="rounded-md p-1 text-[var(--text-tertiary)] hover:bg-[var(--code-bg)] hover:text-[var(--text-primary)]"
              >
                <X className="h-5 w-5" weight="bold" />
              </button>
            </div>

            {/* Key Content */}
            <div className="max-h-[60vh] overflow-y-auto p-6">
              <p className="mb-4 text-sm text-[var(--text-secondary)]">
                Copy this public key to encrypt messages that only we can read.
              </p>
              <pre className="whitespace-pre-wrap break-all rounded-md border border-[var(--border)] bg-[var(--code-bg)] p-4 text-xs text-[var(--text-primary)]">
                {PUBLIC_GPG_KEY}
              </pre>
            </div>

            {/* Modal Footer */}
            <div className="border-t border-[var(--border)] px-6 py-4">
              <button
                onClick={handleCopyKey}
                className="flex w-full items-center justify-center gap-2 rounded-md border border-[var(--border)] bg-[var(--code-bg)] px-4 py-2 text-sm font-medium text-[var(--text-primary)] hover:bg-[var(--border)] transition-colors"
              >
                {copied ? (
                  <>
                    <Check className="h-4 w-4 text-[var(--success)]" weight="bold" />
                    <span>Copied!</span>
                  </>
                ) : (
                  <>
                    <Copy className="h-4 w-4" weight="bold" />
                    <span>Copy key</span>
                  </>
                )}
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
