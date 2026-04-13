import { useEffect, useState, useCallback } from 'react'
import { fetchProcesses, sendCommand } from '../api/client'

const STATE_META = {
  READY:   { cls: 'badge-ready',   dot: 'var(--cyan)',   glow: 'rgba(0,212,255,0.25)'  },
  RUNNING: { cls: 'badge-running', dot: 'var(--green)',  glow: 'rgba(0,255,157,0.25)'  },
  BLOCKED: { cls: 'badge-blocked', dot: 'var(--amber)',  glow: 'rgba(245,158,11,0.25)' },
  WAITING: { cls: 'badge-waiting', dot: 'var(--purple)', glow: 'rgba(168,85,247,0.25)' },
  DONE:    { cls: 'badge-done',    dot: 'var(--muted2)', glow: 'transparent'            },
}

function priLabel(n) {
  if (n >= 8) return { text: 'CRITICAL', color: 'var(--red)'    }
  if (n >= 5) return { text: 'HIGH',     color: 'var(--amber)'  }
  if (n >= 2) return { text: 'NORMAL',   color: 'var(--cyan)'   }
  return             { text: 'LOW',      color: 'var(--muted2)' }
}

function ProcRow({ p, onKill, killing }) {
  const meta = STATE_META[p.state] ?? STATE_META.READY
  const pri  = priLabel(p.priority)
  const pct  = p.burst > 0
    ? Math.max(0, Math.round(((p.burst - p.remaining) / p.burst) * 100))
    : 100

  const barColor =
    p.state === 'RUNNING' ? 'var(--green)'  :
    p.state === 'BLOCKED' ? 'var(--amber)'  :
    p.state === 'WAITING' ? 'var(--purple)' :
    'var(--cyan)'

  return (
    <div className="animate-slide-in" style={{
      padding: '10px 16px',
      borderBottom: '1px solid rgba(30,58,95,0.35)',
      position: 'relative',
      transition: 'background 0.2s',
    }}>
      {/* Running glow accent */}
      {p.state === 'RUNNING' && (
        <div style={{
          position:'absolute', left:0, top:0, bottom:0, width:2,
          background:'var(--green)',
          boxShadow:'0 0 8px var(--green)',
          borderRadius:'0 2px 2px 0'
        }}/>
      )}

      {/* Row 1 */}
      <div style={{ display:'flex', alignItems:'center', justifyContent:'space-between', marginBottom:7 }}>
        <div style={{ display:'flex', alignItems:'center', gap:7, minWidth:0 }}>
          <div style={{
            width:22, height:22, borderRadius:5, flexShrink:0,
            background: `${meta.dot}18`,
            border: `1px solid ${meta.dot}35`,
            display:'flex', alignItems:'center', justifyContent:'center'
          }}>
            <svg width="10" height="10" viewBox="0 0 24 24" fill="none" stroke={meta.dot} strokeWidth="2" strokeLinecap="round">
              <rect x="2" y="3" width="20" height="14" rx="2"/>
              <path d="M8 21h8M12 17v4"/>
            </svg>
          </div>
          <div>
            <div style={{ display:'flex', alignItems:'center', gap:5 }}>
              <span className="font-mono" style={{ fontSize:11, color:'var(--text)', fontWeight:600, maxWidth:90, overflow:'hidden', textOverflow:'ellipsis', whiteSpace:'nowrap' }}>
                {p.name}
              </span>
              <span className="font-mono" style={{ fontSize:8, color:'var(--muted)', background:'rgba(30,58,95,0.5)', padding:'1px 5px', borderRadius:4 }}>
                PID:{p.pid}
              </span>
            </div>
          </div>
        </div>
        <div style={{ display:'flex', alignItems:'center', gap:6 }}>
          <span className={`badge ${meta.cls}`}>
            <span style={{ width:4, height:4, borderRadius:'50%', background:meta.dot, display:'inline-block', flexShrink:0 }}/>
            {p.state}
          </span>
          <button
            id={`pm-kill-${p.pid}`}
            onClick={() => onKill(p.pid)}
            disabled={killing === p.pid}
            aria-label={`Kill process ${p.pid}`}
            style={{
              padding:'2px 7px', borderRadius:5, fontSize:8, fontWeight:700,
              fontFamily:'JetBrains Mono,monospace',
              background:'rgba(255,68,102,0.09)',
              border:'1px solid rgba(255,68,102,0.25)',
              color:'var(--red)', cursor:'pointer',
              transition:'all 0.15s', opacity: killing === p.pid ? 0.4 : 1,
              flexShrink:0
            }}
          >
            {killing === p.pid ? '…' : 'KILL'}
          </button>
        </div>
      </div>

      {/* Row 2: Progress bar */}
      <div style={{ display:'flex', alignItems:'center', gap:8, marginBottom:5 }}>
        <div className="progress-track" style={{ flex:1 }}>
          <div className="progress-fill" style={{ width:`${pct}%`, background: barColor }}/>
        </div>
        <span className="font-mono" style={{ fontSize:8, color:'var(--muted)', flexShrink:0, minWidth:44, textAlign:'right' }}>
          {p.remaining}/{p.burst}u
        </span>
      </div>

      {/* Row 3: chip info */}
      <div style={{ display:'flex', alignItems:'center', gap:8 }}>
        <span className="font-mono" style={{ fontSize:8, color: pri.color }}>
          ▲ {pri.text}
        </span>
        <span className="font-mono" style={{ fontSize:8, color:'var(--muted)' }}>·</span>
        <span className="font-mono" style={{ fontSize:8, color:'var(--muted)' }}>{pct}% done</span>
        <span className="font-mono" style={{ fontSize:8, color:'var(--muted)' }}>·</span>
        <span className="font-mono" style={{ fontSize:8, color:'var(--muted)' }}>quantum=3</span>
      </div>
    </div>
  )
}

function Empty() {
  return (
    <div style={{ display:'flex', flexDirection:'column', alignItems:'center', justifyContent:'center', padding:'24px 16px', gap:8, opacity:0.4 }}>
      <div style={{
        width:44, height:44, borderRadius:12,
        background:'rgba(30,58,95,0.3)',
        border:'1px solid rgba(30,58,95,0.5)',
        display:'flex', alignItems:'center', justifyContent:'center'
      }}>
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="var(--muted2)" strokeWidth="1.4" strokeLinecap="round">
          <rect x="2" y="3" width="20" height="14" rx="2"/>
          <path d="M8 21h8M12 17v4M5 7l3 3-3 3M11 13h5"/>
        </svg>
      </div>
      <span className="font-mono text-[11px]" style={{ color:'var(--muted2)' }}>No active processes</span>
      <span className="font-mono text-[9px]" style={{ color:'var(--muted)', opacity:0.7 }}>try: run init 10 3</span>
    </div>
  )
}

export default function ProcessMonitor({ refresh }) {
  const [procs,   setProcs]   = useState([])
  const [loading, setLoading] = useState(true)
  const [error,   setError]   = useState(null)
  const [killing, setKilling] = useState(null)

  const load = useCallback(async () => {
    try {
      const data = await fetchProcesses()
      setProcs(data.processes ?? [])
      setError(null)
    } catch {
      setError('Bridge unreachable')
    } finally {
      setLoading(false)
    }
  }, [])

  useEffect(() => { load() }, [refresh, load])
  useEffect(() => {
    const id = setInterval(load, 2500)
    return () => clearInterval(id)
  }, [load])

  const handleKill = useCallback(async (pid) => {
    setKilling(pid)
    try {
      await sendCommand(`kill ${pid}`)
      await load()
    } finally {
      setKilling(null)
    }
  }, [load])

  const running = procs.filter(p => p.state === 'RUNNING').length
  const ready   = procs.filter(p => p.state === 'READY').length
  const blocked = procs.filter(p => p.state === 'BLOCKED').length

  return (
    <div className="panel flex flex-col min-h-0">

      {/* Header */}
      <div className="panel-header">
        <div className="flex items-center gap-2">
          <div style={{
            width:22, height:22, borderRadius:6,
            background:'rgba(0,255,157,0.08)',
            border:'1px solid rgba(0,255,157,0.2)',
            display:'flex', alignItems:'center', justifyContent:'center'
          }}>
            <svg width="11" height="11" viewBox="0 0 24 24" fill="none"
                 stroke="var(--green)" strokeWidth="2" strokeLinecap="round">
              <rect x="2" y="3" width="20" height="14" rx="2"/>
              <path d="M8 21h8M12 17v4M5 7l3 3-3 3M11 13h5"/>
            </svg>
          </div>
          <span className="panel-title">Scheduler</span>
          <span className="font-mono text-[8px] px-1.5 py-0.5 rounded"
                style={{ background:'rgba(0,255,157,0.07)', color:'var(--green)', border:'1px solid rgba(0,255,157,0.15)' }}>
            Round-Robin
          </span>
        </div>
        <div className="flex items-center gap-2">
          {/* State counters */}
          {procs.length > 0 && (
            <div style={{ display:'flex', gap:6 }}>
              {running > 0 && <span className="font-mono text-[8px]" style={{ color:'var(--green)' }}>●{running} run</span>}
              {ready   > 0 && <span className="font-mono text-[8px]" style={{ color:'var(--cyan)'  }}>●{ready} rdy</span>}
              {blocked > 0 && <span className="font-mono text-[8px]" style={{ color:'var(--amber)' }}>●{blocked} blk</span>}
            </div>
          )}
          {loading && <span className="font-mono text-[8px] animate-pulse" style={{ color:'var(--muted)' }}>sync…</span>}
          <span id="pm-proc-count"
                className="font-mono text-[9px] px-2 py-0.5 rounded-full"
                style={{
                  background:'rgba(0,255,157,0.07)',
                  color:'var(--green)',
                  border:'1px solid rgba(0,255,157,0.18)'
                }}>
            {procs.length}<span style={{ color:'var(--muted)' }}>/16</span>
          </span>
        </div>
      </div>

      {/* Process list */}
      <div className="flex-1 overflow-y-auto">
        {error && (
          <div style={{ padding:'10px 16px' }}>
            <span style={{ color:'var(--red)', fontFamily:'JetBrains Mono', fontSize:10 }}>✗ {error}</span>
          </div>
        )}
        {!error && !loading && procs.length === 0 && <Empty/>}
        {!error && procs.map(p => (
          <ProcRow key={p.pid} p={p} onKill={handleKill} killing={killing}/>
        ))}
      </div>

      {/* Footer */}
      {procs.length > 0 && (
        <div style={{
          padding:'7px 16px',
          borderTop:'1px solid rgba(30,58,95,0.4)',
          display:'flex', alignItems:'center', justifyContent:'space-between',
          flexShrink:0
        }}>
          <span className="font-mono text-[8px]" style={{ color:'var(--muted)' }}>
            Algorithm: Round-Robin
          </span>
          <span className="font-mono text-[8px]" style={{ color:'var(--muted)' }}>
            Time Quantum: 3 units
          </span>
        </div>
      )}
    </div>
  )
}
