import { useState, useRef, useEffect, useCallback } from 'react'
import { sendCommand } from '../api/client'

const PROMPT = 'root@mini-os:~$'

function colouriseCmd(raw) {
  const parts = raw.split(' ')
  return parts.map((p, i) => {
    if (i === 0) return { text: p, color: 'var(--cyan)',   bold: true  }
    if (p.startsWith('-')) return { text: p, color: 'var(--amber)',  bold: false }
    if (/^\d+$/.test(p))  return { text: p, color: 'var(--purple)', bold: false }
    return                       { text: p, color: '#c8daf5',        bold: false }
  })
}

const BOOT_LINES = [
  { t: 'system', s: '┌─────────────────────────────────────────────┐' },
  { t: 'system', s: '│         MiniOS v2.0  ─  Live Kernel          │' },
  { t: 'system', s: '│   C From Scratch · No stdlib · No malloc     │' },
  { t: 'system', s: '│   Shell · VFS · Scheduler · MemMgr · Parser  │' },
  { t: 'system', s: '└─────────────────────────────────────────────┘' },
  { t: 'info',   s: '  ✓  Memory manager       initialised  [65536 B VRAM]' },
  { t: 'info',   s: '  ✓  Virtual filesystem   mounted      [64 inode capacity]' },
  { t: 'info',   s: '  ✓  Process scheduler    ready        [Round-Robin, q=3]' },
  { t: 'info',   s: '  ✓  Command parser        active      [12 commands]' },
  { t: 'info',   s: '  ✓  Node.js bridge        connected   [:3001]' },
  { t: 'system', s: '' },
  { t: 'system', s: '  Type "help" to list all commands.' },
]

const CMD_COMPLETIONS = [
  'help', 'create', 'write', 'read', 'ls', 'delete',
  'run', 'ps', 'tick', 'kill', 'mem', 'status',
  'echo', 'math', 'clear', 'exit',
]

const QUICK_CMDS = [
  { label: 'help',   cmd: 'help' },
  { label: 'ls',     cmd: 'ls' },
  { label: 'ps',     cmd: 'ps' },
  { label: 'mem',    cmd: 'mem' },
  { label: 'status', cmd: 'status' },
  { label: 'tick',   cmd: 'tick' },
]

function OutputLine({ line, idx }) {
  const cls =
    line.type === 'output'  ? 'term-output'  :
    line.type === 'error'   ? 'term-error'   :
    line.type === 'info'    ? 'term-info'    :
    line.type === 'success' ? 'term-output-success' :
    'term-system'

  if (line.type === 'cmd') {
    const segs = colouriseCmd(line.text)
    return (
      <div className="term-line animate-slide-in" style={{ marginTop: 6 }}>
        <span className="term-prompt mr-2">{PROMPT}</span>
        {segs.map((s, si) => (
          <span key={si} style={{
            color: s.color,
            fontWeight: s.bold ? 700 : 400,
            marginRight: si < segs.length - 1 ? 6 : 0
          }}>{s.text}</span>
        ))}
      </div>
    )
  }

  return (
    <div className="term-line animate-slide-in">
      <span className={cls}>{line.text}</span>
    </div>
  )
}

export default function Terminal({ onCommandRan }) {
  const [lines,   setLines]   = useState([])
  const [input,   setInput]   = useState('')
  const [loading, setLoading] = useState(false)
  const [history, setHistory] = useState([])
  const [histIdx, setHistIdx] = useState(-1)
  const [booted,  setBooted]  = useState(false)
  const [cmdCount, setCmdCount] = useState(0)

  const bottomRef = useRef(null)
  const inputRef  = useRef(null)

  /* Boot sequence */
  useEffect(() => {
    if (booted) return
    setBooted(true)
    BOOT_LINES.forEach((entry, i) => {
      setTimeout(() => {
        setLines(prev => [...prev, { type: entry.t, text: entry.s }])
      }, i * 55)
    })
  }, [booted])

  /* Auto-scroll */
  useEffect(() => {
    bottomRef.current?.scrollIntoView({ behavior: 'smooth' })
  }, [lines, loading])

  /* Focus on mount */
  useEffect(() => { inputRef.current?.focus() }, [])

  const pushLine = useCallback((entry) => setLines(prev => [...prev, entry]), [])

  const runCmd = useCallback(async (cmd) => {
    if (!cmd || loading) return
    setHistory(h => [cmd, ...h.slice(0, 99)])
    setHistIdx(-1)
    setInput('')
    pushLine({ type: 'cmd', text: cmd })

    if (cmd.trim() === 'clear') {
      setLines([])
      return
    }

    setLoading(true)
    try {
      const { output } = await sendCommand(cmd)
      if (output === '__CLEAR__') {
        setLines([])
      } else if (output) {
        const parts = output.split('\n').filter(l => l.length > 0)
        const isSuccess = cmd.startsWith('create') || cmd.startsWith('write') ||
                          cmd.startsWith('delete') || cmd.startsWith('run') ||
                          cmd.startsWith('kill')
        setLines(prev => [
          ...prev,
          ...parts.map(text => ({
            type: text.startsWith('Error') ? 'error' :
                  text.startsWith('=') || text.startsWith('-') ? 'system' :
                  isSuccess && !text.startsWith(' ') ? 'success' : 'output',
            text
          }))
        ])
      }
      setCmdCount(c => c + 1)
      onCommandRan && onCommandRan(cmd)
    } catch (err) {
      pushLine({ type: 'error', text: `Bridge error: ${err.message}` })
    } finally {
      setLoading(false)
      setTimeout(() => inputRef.current?.focus(), 30)
    }
  }, [loading, pushLine, onCommandRan])

  const handleSubmit = useCallback(async (e) => {
    e.preventDefault()
    await runCmd(input.trim())
  }, [input, runCmd])

  const handleKeyDown = useCallback((e) => {
    if (e.key === 'ArrowUp') {
      e.preventDefault()
      const idx = Math.min(histIdx + 1, history.length - 1)
      setHistIdx(idx)
      setInput(history[idx] ?? '')
    } else if (e.key === 'ArrowDown') {
      e.preventDefault()
      const idx = Math.max(histIdx - 1, -1)
      setHistIdx(idx)
      setInput(idx === -1 ? '' : history[idx])
    } else if (e.key === 'Tab') {
      e.preventDefault()
      const match = CMD_COMPLETIONS.find(c => c.startsWith(input.trim()))
      if (match) setInput(match + ' ')
    } else if (e.key === 'l' && e.ctrlKey) {
      e.preventDefault()
      setLines([])
    }
  }, [histIdx, history, input])

  return (
    <div className="panel flex flex-col h-full">

      {/* Header */}
      <div className="panel-header">
        <div className="flex items-center gap-3">
          <div className="traffic-lights">
            <div className="tl-dot tl-red"    title="Close"/>
            <div className="tl-dot tl-yellow" title="Minimise"/>
            <div className="tl-dot tl-green"  title="Maximise"/>
          </div>
          <span className="panel-title">Terminal</span>
          <span className="font-mono text-[8px] px-1.5 py-0.5 rounded"
                style={{ background:'rgba(0,212,255,0.07)', color:'var(--muted2)', border:'1px solid rgba(0,212,255,0.12)' }}>
            bash-sim
          </span>
        </div>
        <div className="flex items-center gap-3">
          <span className="font-mono text-[9px]" style={{ color:'var(--muted)' }}>
            {cmdCount} cmd{cmdCount !== 1 ? 's' : ''}
          </span>
          <button
            onClick={() => setLines([])}
            title="Clear terminal (Ctrl+L)"
            className="icon-btn"
            aria-label="Clear terminal"
          >
            <svg width="10" height="10" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round">
              <path d="M3 6h18M8 6V4h8v2M19 6l-1 14H6L5 6"/>
            </svg>
          </button>
          <div className="flex items-center gap-1.5">
            <span className="online-dot"/>
            <span className="font-mono text-[9px] font-bold tracking-wider"
                  style={{ color:'var(--green)' }}>LIVE</span>
          </div>
        </div>
      </div>

      {/* Quick commands bar */}
      <div style={{
        display:'flex', alignItems:'center', gap:6, padding:'6px 16px',
        background:'rgba(4,8,16,0.6)',
        borderBottom:'1px solid rgba(30,58,95,0.4)',
        flexShrink:0, flexWrap:'wrap'
      }}>
        <span className="font-mono text-[8px] tracking-wider" style={{ color:'var(--muted)', marginRight:2 }}>QUICK:</span>
        {QUICK_CMDS.map(q => (
          <button key={q.cmd} className="cmd-chip" onClick={() => runCmd(q.cmd)} title={q.cmd}>
            {q.label}
          </button>
        ))}
      </div>

      {/* Output window */}
      <div
        id="terminal-output"
        className="terminal-body flex-1"
        onClick={() => inputRef.current?.focus()}
        role="log"
        aria-live="polite"
        aria-label="Terminal output"
      >
        {lines.map((line, i) => <OutputLine key={i} line={line} idx={i} />)}

        {loading && (
          <div className="term-line" style={{ marginTop: 6 }}>
            <span className="term-prompt mr-2">{PROMPT}</span>
            <span style={{ color:'var(--cyan)', opacity:0.8 }} className="animate-pulse">
              executing
            </span>
            <span style={{
              color:'var(--cyan)',
              animation:'blink 0.9s step-end infinite',
              marginLeft:4
            }}>█</span>
          </div>
        )}

        <div ref={bottomRef} />
      </div>

      {/* Input row */}
      <form
        id="terminal-input-form"
        className="term-input-row"
        onSubmit={handleSubmit}
      >
        <span className="term-prompt flex-shrink-0">{PROMPT}</span>
        <input
          id="terminal-input"
          ref={inputRef}
          className="term-input"
          value={input}
          onChange={e => setInput(e.target.value)}
          onKeyDown={handleKeyDown}
          disabled={loading}
          placeholder="enter command… (Tab=autocomplete  ↑↓=history  Ctrl+L=clear)"
          spellCheck={false}
          autoComplete="off"
          autoCorrect="off"
          autoCapitalize="off"
          aria-label="Command input"
        />
        <button
          id="terminal-run-btn"
          type="submit"
          disabled={loading || !input.trim()}
          className="run-btn"
        >
          {loading ? '…' : 'RUN'}
        </button>
      </form>
    </div>
  )
}
