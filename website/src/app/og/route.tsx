import { ImageResponse } from 'next/og';

export const runtime = 'edge';

const SQUIRCLE = 'M 198 100 C 198 3 197 2 100 2 C 3 2 2 3 2 100 C 2 197 3 198 100 198 C 197 198 198 197 198 100 Z';

const ICON =
  'M 1.04 -0.065 L 40.429 -0.84 L 40.02 39.627 L 0.258 40.029 L 1.04 -0.065 Z ' +
  'M 39.96 47.689 C 39.993 43.238 43.573 39.579 47.958 39.52 L 71.879 39.227 C 76.306 39.19 79.887 42.799 79.875 47.281 L 79.848 71.651 C 79.853 76.138 76.264 79.785 71.827 79.796 L 47.673 79.82 C 43.222 79.825 39.668 76.233 39.73 71.801 L 39.96 47.689 Z ' +
  'M -0.646 79.918 L 39.613 79.829 L 39.428 120.233 L -0.958 120.144 L -0.646 79.918 Z ' +
  'M 39.545 128.286 C 39.477 123.842 43.05 120.236 47.52 120.239 L 71.794 120.228 C 76.254 120.224 79.871 123.841 79.878 128.307 L 79.922 152.595 C 79.935 157.065 76.367 160.653 71.954 160.61 L 48.025 160.402 C 43.619 160.371 39.993 156.761 39.927 152.339 L 39.545 128.286 Z ' +
  'M 120.078 47.405 C 120.065 42.935 123.634 39.347 128.046 39.39 L 151.975 39.598 C 156.38 39.629 160.007 43.238 160.073 47.661 L 160.455 71.714 C 160.523 76.158 156.949 79.764 152.48 79.761 L 128.206 79.772 C 123.747 79.776 120.129 76.159 120.122 71.693 L 120.078 47.405 Z ' +
  'M 79.781 100 C 79.856 88.832 88.853 79.727 100 79.713 C 111.128 79.76 120.178 88.813 120.219 100 C 120.144 111.169 111.147 120.273 100 120.287 C 88.872 120.24 79.822 111.188 79.781 100 Z ' +
  'M -0.048 160.155 L 40.046 160.347 L 40.617 200.523 L 1.088 200.017 L -0.048 160.155 Z ' +
  'M 160.572 79.767 L 200.958 79.856 L 200.646 120.082 L 160.387 120.171 L 160.572 79.767 Z ' +
  'M 80.097 -1.296 L 119.859 -1.073 L 120.053 39.312 L 79.899 39.167 L 80.097 -1.296 Z ' +
  'M 120.152 128.349 C 120.147 123.862 123.737 120.215 128.173 120.204 L 152.327 120.18 C 156.778 120.175 160.332 123.767 160.27 128.199 L 160.04 152.311 C 160.007 156.762 156.427 160.421 152.042 160.48 L 128.121 160.773 C 123.695 160.81 120.113 157.201 120.125 152.719 L 120.152 128.349 Z ' +
  'M 79.947 160.688 L 120.101 160.833 L 119.903 201.296 L 80.141 201.073 L 79.947 160.688 Z ' +
  'M 159.383 -0.523 L 198.912 -0.017 L 200.048 39.845 L 159.954 39.653 L 159.383 -0.523 Z ' +
  'M 159.98 160.373 L 199.742 159.971 L 198.96 200.065 L 159.571 200.84 L 159.98 160.373 Z';

export async function GET(request: Request) {
  const { searchParams } = new URL(request.url);

  const title = searchParams.get('title') ?? 'OpenSoundDeck';
  const description =
    searchParams.get('description') ??
    'The open source soundboard for voice chats and streams.';
  const label = searchParams.get('label') ?? '';
  const tagsRaw = searchParams.get('tags') ?? 'Open Source,Cross-Platform,Free';
  const tags = tagsRaw.split(',').filter(Boolean).slice(0, 4);

  const titleSize = title.length > 42 ? 50 : title.length > 28 ? 60 : 70;
  const desc = description.length > 140 ? description.slice(0, 137) + '…' : description;

  return new ImageResponse(
    (
      <div
        style={{
          display: 'flex',
          width: '100%',
          height: '100%',
          background: '#070D1E',
          overflow: 'hidden',
          position: 'relative',
          fontFamily: 'sans-serif',
        }}
      >
        {/* Dot grid */}
        <div
          style={{
            position: 'absolute',
            inset: 0,
            backgroundImage:
              'radial-gradient(circle, rgba(96,165,250,0.18) 1px, transparent 1px)',
            backgroundSize: '36px 36px',
          }}
        />

        {/* Top-right blue-cyan glow */}
        <div
          style={{
            position: 'absolute',
            top: -280,
            right: -180,
            width: 720,
            height: 720,
            borderRadius: '50%',
            background:
              'radial-gradient(circle, rgba(56,189,248,0.35) 0%, rgba(96,165,250,0.18) 35%, transparent 65%)',
          }}
        />

        {/* Bottom-left purple accent */}
        <div
          style={{
            position: 'absolute',
            bottom: -160,
            left: -80,
            width: 480,
            height: 480,
            borderRadius: '50%',
            background:
              'radial-gradient(circle, rgba(129,140,248,0.18) 0%, transparent 60%)',
          }}
        />

        {/* Center subtle glow */}
        <div
          style={{
            position: 'absolute',
            top: '40%',
            left: '40%',
            width: 300,
            height: 300,
            borderRadius: '50%',
            background:
              'radial-gradient(circle, rgba(96,165,250,0.06) 0%, transparent 70%)',
          }}
        />

        {/* Ghost icon watermark (right side) */}
        <div
          style={{
            position: 'absolute',
            right: -60,
            top: '50%',
            transform: 'translateY(-50%)',
            display: 'flex',
            opacity: 0.04,
          }}
        >
          <svg width="480" height="480" viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
            <path d={SQUIRCLE} fill="white" />
            <g transform="translate(100 100) scale(0.75) translate(-100 -100)">
              <path d={ICON} fill="#60A5FA" />
            </g>
          </svg>
        </div>

        {/* Decorative circles — top right area */}
        <div style={{ position: 'absolute', top: 44, right: 72, display: 'flex', gap: 14, alignItems: 'center' }}>
          {([44, 28, 18] as const).map((size, i) => (
            <div
              key={i}
              style={{
                width: size,
                height: size,
                borderRadius: '50%',
                background: `rgba(56,189,248,${0.18 - i * 0.05})`,
                border: `1.5px solid rgba(56,189,248,${0.4 - i * 0.1})`,
              }}
            />
          ))}
        </div>

        {/* ── Main content ── */}
        <div
          style={{
            display: 'flex',
            flexDirection: 'column',
            padding: '52px 80px 56px 80px',
            width: '100%',
            height: '100%',
            position: 'relative',
          }}
        >
          {/* Logo + brand */}
          <div style={{ display: 'flex', alignItems: 'center', gap: 20 }}>
            <svg width="60" height="60" viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
              <path d={SQUIRCLE} fill="white" />
              <g transform="translate(100 100) scale(0.75) translate(-100 -100)">
                <path d={ICON} fill="#60A5FA" />
              </g>
            </svg>
            <div style={{ display: 'flex', flexDirection: 'column', gap: 4 }}>
              <span
                style={{
                  color: '#60A5FA',
                  fontSize: 22,
                  fontWeight: 800,
                  letterSpacing: '-0.02em',
                }}
              >
                OpenSoundDeck
              </span>
              <span style={{ color: '#1E3A5F', fontSize: 15, letterSpacing: '0.02em' }}>
                opensounddeck.vercel.app
              </span>
            </div>
          </div>

          {/* Separator */}
          <div
            style={{
              display: 'flex',
              height: 1,
              marginTop: 36,
              marginBottom: 36,
              background:
                'linear-gradient(90deg, rgba(56,189,248,0.7) 0%, rgba(96,165,250,0.3) 40%, transparent 100%)',
            }}
          />

          {/* Label */}
          {label ? (
            <div
              style={{
                display: 'flex',
                marginBottom: 14,
                color: '#38BDF8',
                fontSize: 16,
                fontWeight: 700,
                letterSpacing: '0.12em',
                textTransform: 'uppercase',
              }}
            >
              {label}
            </div>
          ) : null}

          {/* Title */}
          <div
            style={{
              display: 'flex',
              color: '#F0F9FF',
              fontSize: titleSize,
              fontWeight: 800,
              letterSpacing: '-0.03em',
              lineHeight: 1.08,
              marginBottom: 20,
              maxWidth: 860,
            }}
          >
            {title}
          </div>

          {/* Description */}
          <div
            style={{
              display: 'flex',
              color: '#475569',
              fontSize: 23,
              lineHeight: 1.55,
              flex: 1,
              maxWidth: 820,
            }}
          >
            {desc}
          </div>

          {/* Tags row */}
          <div style={{ display: 'flex', alignItems: 'center', gap: 12, marginTop: 32 }}>
            {tags.map((tag, i) => (
              <div
                key={i}
                style={{
                  display: 'flex',
                  padding: '9px 20px',
                  background: 'rgba(14,30,60,0.9)',
                  border: '1px solid rgba(96,165,250,0.38)',
                  borderRadius: 8,
                  color: '#7DD3FC',
                  fontSize: 16,
                  fontWeight: 600,
                  letterSpacing: '0.01em',
                }}
              >
                {tag}
              </div>
            ))}
            <div style={{ flex: 1 }} />
            {/* MIT badge */}
            <div
              style={{
                display: 'flex',
                alignItems: 'center',
                gap: 8,
                padding: '9px 20px',
                background: 'rgba(56,189,248,0.07)',
                border: '1px solid rgba(56,189,248,0.22)',
                borderRadius: 8,
                color: '#38BDF8',
                fontSize: 16,
                fontWeight: 600,
              }}
            >
              MIT License
            </div>
          </div>
        </div>
      </div>
    ),
    { width: 1200, height: 630 },
  );
}
