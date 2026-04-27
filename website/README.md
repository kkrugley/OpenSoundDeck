# OpenSoundDeck Website

The official website for OpenSoundDeck - an open-source, cross-platform soundboard application.

## Tech Stack

- **Framework:** Next.js 15 (App Router)
- **Language:** TypeScript
- **Styling:** Tailwind CSS
- **UI Icons:** Phosphor Icons
- **Build Output:** Static HTML export

## Project Structure

```
website/
├── src/
│   ├── app/              # Next.js App Router pages
│   │   ├── page.tsx      # Homepage
│   │   ├── layout.tsx    # Root layout
│   │   ├── docs/         # Documentation page
│   │   ├── download/     # Downloads page
│   │   ├── help/         # Contact form with GPG encryption
│   │   └── support/      # Donation/support page
│   ├── components/       # React components
│   │   ├── Header.tsx
│   │   ├── Footer.tsx
│   │   ├── Hero.tsx
│   │   ├── Features.tsx
│   │   ├── Statistics.tsx
│   │   ├── Privacy.tsx
│   │   ├── FAQ.tsx
│   │   ├── SoundEngine.tsx
│   │   ├── Newsletter.tsx
│   │   ├── EncryptedContactForm.tsx
│   │   └── DonationTabs.tsx
│   └── lib/
│       ├── constants.ts  # Project constants, GPG key, FAQ
│       └── utils.ts      # Utility functions
├── .env.local            # Environment variables (Web3Forms key)
└── next.config.ts        # Next.js configuration
```

## Features

- **Homepage** - Hero section, features list, statistics, FAQ, newsletter signup
- **Documentation** - Installation guides, configuration, troubleshooting
- **Downloads** - Platform-specific download links and package manager commands
- **Help** - Encrypted contact form using PGP/OpenPGP.js
- **Support** - Donation methods with vertical tabs (GitHub Sponsors, Crypto, PayPal, Ko-fi)
- **i18n Ready** - Language switcher (EN/RU/PL) with UI translations
- **GitHub Integration** - Live stats fetching from GitHub API

## Getting Started

### Prerequisites

- Node.js 18+
- npm or yarn

### Installation

```bash
cd website
npm install
```

### Development

```bash
npm run dev
```

Open [http://localhost:3000](http://localhost:3000) to view the site.

### Build

```bash
npm run build
```

The static site will be exported to the `dist/` directory.

## Environment Variables

Copy `.env.example` to `.env.local` and add your Web3Forms access key:

```env
NEXT_PUBLIC_WEB3FORMS_KEY=your_access_key_here
```

Get your key from [Web3Forms](https://web3forms.com/).

## GPG Encryption

The contact form on `/help` uses OpenPGP.js to encrypt messages in the browser before sending them via Web3Forms. The public GPG key is stored in `src/lib/constants.ts`.

## Design System

Based on the OpenCode.ai design:

- **Background:** `#F5F5F5`
- **Text Primary:** `#1A1A1A`
- **Text Secondary:** `#666666`
- **Accent:** `#D4A14F` (golden)
- **Font:** JetBrains Mono / Monospace
- **Border Radius:** 4-6px
- **Shadows:** Minimal, flat design approach

## Pages

| Page | Description |
|------|-------------|
| `/` | Homepage with all sections |
| `/docs` | Documentation with sidebar navigation |
| `/download` | Download links for all platforms |
| `/help` | Encrypted contact form |
| `/support` | Donation/support page |

## License

This website is part of the OpenSoundDeck project and is licensed under the MIT License.
