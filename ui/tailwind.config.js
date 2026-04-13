/** @type {import('tailwindcss').Config} */
export default {
  content: ['./index.html', './src/**/*.{js,jsx}'],
  theme: {
    extend: {
      colors: {
        'os-bg':       '#060b14',
        'os-surface':  '#0d1a2d',
        'os-surface2': '#112240',
        'os-border':   '#1e3a5f',
        'os-cyan':     '#00d4ff',
        'os-green':    '#00ff9d',
        'os-amber':    '#fbbf24',
        'os-red':      '#ff4466',
        'os-purple':   '#a855f7',
        'os-pink':     '#ec4899',
        'os-muted':    '#4a6fa5',
        'os-text':     '#cdd9f0',
      },
      fontFamily: {
        mono: ['"JetBrains Mono"', '"Fira Code"', 'Consolas', 'monospace'],
        sans: ['"Inter"', 'system-ui', 'sans-serif'],
      },
      boxShadow: {
        'glow-cyan':  '0 0 20px rgba(0,212,255,0.2)',
        'glow-green': '0 0 20px rgba(0,255,157,0.2)',
        'glow-panel': '0 8px 40px rgba(0,0,0,0.6)',
      },
      animation: {
        'pulse-slow': 'pulse 3s cubic-bezier(0.4,0,0.6,1) infinite',
        'blink':      'blink 1s step-end infinite',
        'slide-in':   'slideIn 0.12s ease-out',
        'fade-in':    'fadeIn 0.3s ease-out',
        'fade-up':    'fadeUp 0.4s ease-out',
        'spin-slow':  'spin-slow 8s linear infinite',
        'ping-dot':   'ping-dot 2s ease infinite',
      },
      keyframes: {
        blink:    { '0%,100%': { opacity: 1 }, '50%': { opacity: 0 } },
        slideIn:  { from: { opacity: 0, transform: 'translateY(5px)' },
                    to:   { opacity: 1, transform: 'translateY(0)' } },
        fadeIn:   { from: { opacity: 0 }, to: { opacity: 1 } },
        fadeUp:   { from: { opacity: 0, transform: 'translateY(8px)' },
                    to:   { opacity: 1, transform: 'translateY(0)' } },
        'spin-slow': {
          from: { transform: 'rotate(0deg)' },
          to:   { transform: 'rotate(360deg)' },
        },
        'ping-dot': {
          '0%,100%': { opacity: 1 },
          '50%':     { opacity: 0.5 },
        },
      },
    }
  },
  plugins: []
}
