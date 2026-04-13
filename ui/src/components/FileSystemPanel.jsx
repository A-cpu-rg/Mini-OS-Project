import { useEffect, useState, useCallback } from 'react'
import { fetchFiles, sendCommand } from '../api/client'

const EXT_META = {
  txt:  { color: 'var(--cyan)',   icon: '📄', label: 'TXT' },
  log:  { color: 'var(--amber)',  icon: '📋', label: 'LOG' },
  bin:  { color: 'var(--purple)', icon: '⚙', label: 'BIN' },
  sh:   { color: 'var(--green)',  icon: '🔧', label: 'SH'  },
  c:    { color: 'var(--cyan)',   icon: '📝', label: 'C'   },
  h:    { color: 'var(--pink)',   icon: '📎', label: 'H'   },
  json: { color: 'var(--amber)',  icon: '{}', label: 'JSON'},
  md:   { color: 'var(--green)',  icon: '✏', label: 'MD'  },
}

function getExt(name) {
  const parts = name.split('.')
  return parts.length > 1 ? parts.pop().toLowerCase() : ''
}
function getMeta(name) {
  return EXT_META[getExt(name)] ?? { color: 'var(--muted2)', icon: '📁', label: 'FILE' }
}

function formatBytes(n) {
  if (n >= 1024) return (n / 1024).toFixed(1) + ' KB'
  return n + ' B'
}

function FileIcon({ name }) {
  const meta = getMeta(name)
  return (
    <div style={{
      width: 28, height: 28, borderRadius: 7, flexShrink: 0,
      background: `${meta.color}14`,
      border: `1px solid ${meta.color}30`,
      display: 'flex', alignItems: 'center', justifyContent: 'center',
    }}>
      <svg width="13" height="13" viewBox="0 0 24 24" fill="none"
           stroke={meta.color} strokeWidth="1.8" strokeLinecap="round" strokeLinejoin="round">
        <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/>
        <polyline points="14 2 14 8 20 8"/>
      </svg>
    </div>
  )
}

function SizeBar({ size, maxSize, color }) {
  const pct = maxSize > 0 ? Math.min(100, Math.round((size / maxSize) * 100)) : 0
  return (
    <div style={{ width: 48, height: 3, background: 'rgba(14,28,56,0.9)', borderRadius: 99, overflow: 'hidden' }}>
      <div style={{
        height: '100%', width: `${pct}%`,
        background: color,
        borderRadius: 99,
        transition: 'width 0.5s ease'
      }}/>
    </div>
  )
}

function EmptyFS() {
  return (
    <div style={{ display:'flex', flexDirection:'column', alignItems:'center', justifyContent:'center', padding:'24px 16px', gap:8, opacity:0.45 }}>
      <div style={{
        width: 44, height: 44, borderRadius: 12,
        background: 'rgba(30,58,95,0.3)',
        border: '1px solid rgba(30,58,95,0.5)',
        display: 'flex', alignItems: 'center', justifyContent: 'center'
      }}>
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="var(--muted2)" strokeWidth="1.4" strokeLinecap="round">
          <path d="M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z"/>
        </svg>
      </div>
      <span className="font-mono text-[11px]" style={{ color:'var(--muted2)' }}>Empty filesystem</span>
      <span className="font-mono text-[9px]" style={{ color:'var(--muted)', opacity:0.7 }}>try: create notes.txt</span>
    </div>
  )
}

export default function FileSystemPanel({ refresh }) {
  const [files,    setFiles]    = useState([])
  const [loading,  setLoading]  = useState(true)
  const [error,    setError]    = useState(null)
  const [deleting, setDeleting] = useState(null)
  const [selected, setSelected] = useState(null)

  const load = useCallback(async () => {
    try {
      const data = await fetchFiles()
      setFiles(data.files ?? [])
      setError(null)
    } catch {
      setError('Bridge unreachable')
    } finally {
      setLoading(false)
    }
  }, [])

  useEffect(() => { load() }, [refresh, load])
  useEffect(() => {
    const id = setInterval(load, 3000)
    return () => clearInterval(id)
  }, [load])

  const handleDelete = useCallback(async (name) => {
    setDeleting(name)
    try {
      await sendCommand(`delete ${name}`)
      await load()
      if (selected === name) setSelected(null)
    } finally {
      setDeleting(null)
    }
  }, [load, selected])

  const maxSize    = files.length > 0 ? Math.max(...files.map(f => f.size), 1) : 1
  const totalBytes = files.reduce((a, f) => a + f.size, 0)
  const fillPct    = Math.round((files.length / 64) * 100)

  const freeSlots = 64 - files.length

  return (
    <div className="panel flex flex-col min-h-0" style={{ animation:'glow-pulse 4s ease infinite' }}>

      {/* Header */}
      <div className="panel-header">
        <div className="flex items-center gap-2">
          <div style={{
            width:22, height:22, borderRadius:6,
            background:'rgba(0,212,255,0.08)',
            border:'1px solid rgba(0,212,255,0.2)',
            display:'flex', alignItems:'center', justifyContent:'center'
          }}>
            <svg width="11" height="11" viewBox="0 0 24 24" fill="none"
                 stroke="var(--cyan)" strokeWidth="2" strokeLinecap="round">
              <path d="M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z"/>
            </svg>
          </div>
          <span className="panel-title">Virtual FS</span>
        </div>
        <div className="flex items-center gap-2">
          {loading && (
            <span className="font-mono text-[8px] animate-pulse" style={{ color:'var(--muted)' }}>sync…</span>
          )}
          <span id="fs-file-count"
                className="font-mono text-[9px] px-2 py-0.5 rounded-full"
                style={{
                  background:'rgba(0,212,255,0.07)',
                  color:'var(--cyan)',
                  border:'1px solid rgba(0,212,255,0.18)'
                }}>
            {files.length}<span style={{ color:'var(--muted)' }}>/64</span>
          </span>
        </div>
      </div>

      {/* File list */}
      <div className="flex-1 overflow-y-auto">
        {error && (
          <div style={{ display:'flex', alignItems:'center', gap:6, padding:'10px 16px' }}>
            <span style={{ color:'var(--red)', fontFamily:'JetBrains Mono', fontSize:10 }}>
              ✗ {error}
            </span>
          </div>
        )}

        {!error && !loading && files.length === 0 && <EmptyFS />}

        {!error && files.map((f, i) => {
          const meta   = getMeta(f.name)
          const isSel  = selected === f.name
          return (
            <div
              key={f.name}
              className={`file-card group animate-slide-in ${isSel ? 'selected' : ''}`}
              style={{
                background: isSel ? 'rgba(0,212,255,0.05)' : undefined,
                borderColor: isSel ? 'rgba(0,212,255,0.15)' : undefined,
                animationDelay: `${i * 20}ms`
              }}
              onClick={() => setSelected(isSel ? null : f.name)}
            >
              {/* Left: icon + name */}
              <div style={{ display:'flex', alignItems:'center', gap:10, minWidth:0, flex:1 }}>
                <FileIcon name={f.name}/>
                <div style={{ minWidth:0, flex:1 }}>
                  <div style={{ display:'flex', alignItems:'center', gap:6, marginBottom:2 }}>
                    <span className="font-mono"
                          style={{ fontSize:11, color:'var(--text)', overflow:'hidden', textOverflow:'ellipsis', whiteSpace:'nowrap' }}
                          title={f.name}>
                      {f.name}
                    </span>
                    <span className="font-mono"
                          style={{
                            fontSize:8, color: meta.color, flexShrink:0,
                            background:`${meta.color}12`,
                            border:`1px solid ${meta.color}25`,
                            padding:'1px 5px', borderRadius:4
                          }}>
                      {meta.label}
                    </span>
                  </div>
                  <div style={{ display:'flex', alignItems:'center', gap:6 }}>
                    <SizeBar size={f.size} maxSize={maxSize} color={meta.color}/>
                    <span className="font-mono" style={{ fontSize:9, color:'var(--muted)' }}>
                      {formatBytes(f.size)}
                    </span>
                  </div>
                </div>
              </div>

              {/* Right: actions */}
              <button
                id={`fs-delete-${f.name}`}
                aria-label={`Delete ${f.name}`}
                onClick={(e) => { e.stopPropagation(); handleDelete(f.name) }}
                disabled={deleting === f.name}
                className="group-hover:opacity-100"
                style={{
                  opacity: 0, background:'rgba(255,68,102,0.09)',
                  border:'1px solid rgba(255,68,102,0.25)', borderRadius:6,
                  padding:'2px 7px', color:'var(--red)',
                  fontFamily:'JetBrains Mono,monospace', fontSize:8, fontWeight:700,
                  cursor:'pointer', transition:'all 0.15s', flexShrink: 0, marginLeft:8,
                }}
              >
                {deleting === f.name ? '…' : 'DEL'}
              </button>
            </div>
          )
        })}
      </div>

      {/* Footer: capacity bar */}
      <div style={{
        padding:'8px 16px',
        borderTop:'1px solid rgba(30,58,95,0.4)',
        flexShrink:0,
        display:'flex', alignItems:'center', justifyContent:'space-between'
      }}>
        <span className="font-mono text-[8px]" style={{ color:'var(--muted)' }}>
          {formatBytes(totalBytes)} · {freeSlots} slots free
        </span>
        <div style={{ display:'flex', alignItems:'center', gap:6 }}>
          <div className="progress-track" style={{ width:56, height:3 }}>
            <div className="progress-fill" style={{
              width:`${fillPct}%`,
              background: fillPct > 80 ? 'var(--red)' : fillPct > 50 ? 'var(--amber)' : 'var(--cyan)'
            }}/>
          </div>
          <span className="font-mono text-[8px]" style={{
            color: fillPct > 80 ? 'var(--red)' : fillPct > 50 ? 'var(--amber)' : 'var(--muted)'
          }}>
            {fillPct}%
          </span>
        </div>
      </div>
    </div>
  )
}
