'use client';

import { useState } from 'react';
import { Header } from '@/components/Header';
import { Footer } from '@/components/Footer';
import { Shield, FileText, Scales, GithubLogo } from '@phosphor-icons/react';
import { GITHUB_URL } from '@/lib/constants';
import { cn } from '@/lib/utils';

const LEGAL_TABS = [
  { id: 'privacy', label: 'Privacy Policy', icon: Shield },
  { id: 'terms', label: 'Terms of Service', icon: FileText },
  { id: 'license', label: 'License', icon: Scales },
] as const;

export default function LegalPage() {
  const [currentLang, setCurrentLang] = useState('en');
  const [activeTab, setActiveTab] = useState<typeof LEGAL_TABS[number]['id']>('privacy');

  return (
    <div className="flex min-h-screen flex-col">
      <Header currentLang={currentLang} onLanguageChange={setCurrentLang} />
      
      <main className="flex-1">
        {/* Page Header */}
        <section className="w-full border-b border-[var(--border)] py-16">
          <div className="mx-auto max-w-4xl px-4 text-center sm:px-6 lg:px-8">
            <h1 className="text-3xl font-bold text-[var(--text-primary)] sm:text-4xl">
              Legal
            </h1>
            <p className="mx-auto mt-4 max-w-2xl text-[var(--text-secondary)]">
              Transparency and trust are important to us. Here you will find our 
              privacy policy, terms of service, and license information.
            </p>
          </div>
        </section>

        {/* Legal Content */}
        <section className="w-full py-16">
          <div className="mx-auto max-w-4xl px-4 sm:px-6 lg:px-8">
            <div className="flex flex-col gap-8 lg:flex-row">
              {/* Sidebar Tabs */}
              <div className="flex flex-col gap-2 lg:w-48 lg:flex-shrink-0">
                {LEGAL_TABS.map((tab) => {
                  const Icon = tab.icon;
                  return (
                    <button
                      key={tab.id}
                      onClick={() => setActiveTab(tab.id)}
                      className={cn(
                        "flex items-center gap-3 rounded-md px-4 py-3 text-left text-sm font-medium transition-colors",
                        activeTab === tab.id
                          ? "bg-[var(--accent-dark)] text-[var(--accent-light)]"
                          : "bg-[var(--code-bg)] text-[var(--text-secondary)] hover:bg-[var(--border)] hover:text-[var(--text-primary)]"
                      )}
                    >
                      <Icon className="h-4 w-4" weight="bold" />
                      {tab.label}
                    </button>
                  );
                })}
              </div>

              {/* Content Area */}
              <div className="flex-1">
                {/* Privacy Policy */}
                {activeTab === 'privacy' && (
                  <div className="space-y-8">
                    <div>
                      <h2 className="text-2xl font-bold text-[var(--text-primary)]">
                        Privacy Policy
                      </h2>
                      <p className="mt-4 text-[var(--text-secondary)]">
                        Last updated: {new Date().toLocaleDateString('en-US', { month: 'long', year: 'numeric' })}
                      </p>
                    </div>

                    <div className="space-y-6 text-[var(--text-secondary)]">
                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Data Collection
                        </h3>
                        <p className="mt-2">
                          OpenSoundDeck does not collect, store, or transmit any personal data. 
                          All audio processing and sound playback happens locally on your machine. 
                          We do not use analytics, telemetry, or any tracking mechanisms.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Local Storage
                        </h3>
                        <p className="mt-2">
                          All your settings, hotkeys, and sound library information is stored 
                          locally in your user configuration directory. This data never leaves 
                          your computer.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Audio Data
                        </h3>
                        <p className="mt-2">
                          OpenSoundDeck processes audio in real-time on your device. No audio 
                          data is recorded, transmitted, or stored by the application. Your 
                          microphone input and played sounds remain entirely under your control.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Third-Party Services
                        </h3>
                        <p className="mt-2">
                          The application does not communicate with any external servers or 
                          third-party services. The only network activity is when you explicitly 
                          choose to download virtual audio drivers (Windows/macOS) or check for 
                          updates.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Contact Form
                        </h3>
                        <p className="mt-2">
                          When using our encrypted contact form, your message is encrypted with 
                          PGP in your browser before being sent. Only the intended recipient can 
                          decrypt and read your message.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Changes to This Policy
                        </h3>
                        <p className="mt-2">
                          We may update this privacy policy from time to time. Any changes will 
                          be posted on this page with an updated revision date.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Contact Us
                        </h3>
                        <p className="mt-2">
                          If you have any questions about this privacy policy, please contact us 
                          through our <a href="/help" className="text-[var(--text-primary)] underline">encrypted contact form</a>.
                        </p>
                      </section>
                    </div>
                  </div>
                )}

                {/* Terms of Service */}
                {activeTab === 'terms' && (
                  <div className="space-y-8">
                    <div>
                      <h2 className="text-2xl font-bold text-[var(--text-primary)]">
                        Terms of Service
                      </h2>
                      <p className="mt-4 text-[var(--text-secondary)]">
                        Last updated: {new Date().toLocaleDateString('en-US', { month: 'long', year: 'numeric' })}
                      </p>
                    </div>

                    <div className="space-y-6 text-[var(--text-secondary)]">
                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Acceptance of Terms
                        </h3>
                        <p className="mt-2">
                          By downloading, installing, or using OpenSoundDeck, you agree to be 
                          bound by these Terms of Service. If you do not agree to these terms, 
                          please do not use the software.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          License
                        </h3>
                        <p className="mt-2">
                          OpenSoundDeck is licensed under the GPL-3.0 license. You are free to use,
                          modify, and distribute the software according to the terms of that license.
                          The software is provided &quot;as is&quot;, without warranty of any kind.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Use Responsibly
                        </h3>
                        <p className="mt-2">
                          You agree to use OpenSoundDeck responsibly and in compliance with all 
                          applicable laws. You are solely responsible for any content you play 
                          through the application and how you use it in voice chats or streams.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          No Warranty
                        </h3>
                        <p className="mt-2">
                          The software is provided without any warranty, express or implied. We 
                          do not guarantee that the software will be error-free or that it will 
                          meet your specific requirements. Use at your own risk.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Limitation of Liability
                        </h3>
                        <p className="mt-2">
                          In no event shall the authors or copyright holders be liable for any 
                          claim, damages, or other liability, whether in an action of contract, 
                          tort, or otherwise, arising from, out of, or in connection with the 
                          software or the use or other dealings in the software.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Contributions
                        </h3>
                        <p className="mt-2">
                          By contributing to OpenSoundDeck, you agree that your contributions
                          will be licensed under the same GPL-3.0 license as the project. You retain
                          copyright to your contributions, but grant us and all users the rights
                          to use them under the GPL-3.0 license terms.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Changes to Terms
                        </h3>
                        <p className="mt-2">
                          We reserve the right to modify these terms at any time. Changes will 
                          be effective immediately upon posting to this page. Continued use of 
                          the software constitutes acceptance of the modified terms.
                        </p>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Contact
                        </h3>
                        <p className="mt-2">
                          For questions about these terms, please contact us through our{" "}
                          <a href="/help" className="text-[var(--text-primary)] underline">encrypted contact form</a>.
                        </p>
                      </section>
                    </div>
                  </div>
                )}

                {/* License */}
                {activeTab === 'license' && (
                  <div className="space-y-8">
                    <div>
                      <h2 className="text-2xl font-bold text-[var(--text-primary)]">
                        GPL-3.0 License
                      </h2>
                      <p className="mt-4 text-[var(--text-secondary)]">
                        OpenSoundDeck is released under the GPL-3.0 license.
                        <a href={`${GITHUB_URL}/blob/main/LICENSE`} target="_blank" rel="noopener noreferrer" className="text-[var(--text-primary)] underline">
                          {' '}View the full license on GitHub
                        </a>.
                      </p>
                    </div>

                    <div className="rounded-lg border border-[var(--border)] bg-[var(--code-bg)] p-6">
                      <pre className="whitespace-pre-wrap text-sm text-[var(--text-primary)]">
{`GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007

Copyright (c) ${new Date().getFullYear()} OpenSoundDeck Contributors

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.`}
                      </pre>
                    </div>

                    <div className="space-y-4 text-[var(--text-secondary)]">
                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          What This Means
                        </h3>
                        <ul className="mt-2 list-inside list-disc space-y-2">
                          <li>You can use OpenSoundDeck for personal or commercial purposes</li>
                          <li>You can modify the source code</li>
                          <li>You can distribute your modifications</li>
                          <li>You must include the copyright notice and license</li>
                          <li>Any derivative works must also be licensed under GPL-3.0</li>
                          <li>You cannot include it in proprietary/closed-source software</li>
                        </ul>
                      </section>

                      <section>
                        <h3 className="text-lg font-bold text-[var(--text-primary)]">
                          Third-Party Licenses
                        </h3>
                        <p className="mt-2">
                          OpenSoundDeck uses several open-source libraries. Their licenses 
                          can be found in the repository:
                        </p>
                        <ul className="mt-2 list-inside list-disc space-y-1">
                          <li>miniaudio — Public Domain or MIT License</li>
                          <li>Qt6 — LGPL v3</li>
                          <li>Other dependencies — See repository for details</li>
                        </ul>
                      </section>

                      <a
                        href={`${GITHUB_URL}/blob/main/LICENSE`}
                        target="_blank"
                        rel="noopener noreferrer"
                        className="inline-flex items-center gap-2 rounded-md border border-[var(--border)] px-4 py-2 text-sm font-medium text-[var(--text-primary)] hover:bg-[var(--code-bg)]"
                      >
                        <GithubLogo className="h-4 w-4" weight="bold" />
                        View on GitHub
                      </a>
                    </div>
                  </div>
                )}
              </div>
            </div>
          </div>
        </section>
      </main>
      
      <Footer currentLang={currentLang} onLanguageChange={setCurrentLang} />
    </div>
  );
}
