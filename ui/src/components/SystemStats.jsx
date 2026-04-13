import { useEffect, useState, useCallback } from 'react'
import { fetchStatus } from '../api/client'

/* ── SVG Gauge ring ──────────────────────────────────── */
function GaugeRing({ value, max, size = 56, color, label, sublabel }) {
  const R    = (size / 2) - 7
  const circ = 2 * Math.PI * R
  const pct  = max > 0 ? Math.min(1, value / max) : 0
  const dash = circ * pct
  const gap  = circ - dash

  const stroke =
    pct > 0.85 ? 'var(--red)' :
    pct > 0.55 ? 'var(--amber)' :
    color

  return (
    <div className="stat-card" style={{ minWidth: 72, gap: 6 }}>
      <div style={{ position: 'relative', width: size, height: size }}>
        <svg width={size} height={size} viewBox={`0 0 ${size} ${size}`}>
          {/* Track */}
          <circle
            cx={size/2} cy={size/2} r={R}
            fill="none" stroke="rgba(14,28,56,0.9)" strokeWidth="5"
            style={{ transform:'rotate(-90deg)', transformOrigin:'center' }}
          />
          {/* Fill */}
          <circle
            cx={size/2} cy={size/2} r={R}
            fill="none" stroke={stroke} strokeWidth="5"
            strokeLinecap="round"
            strokeDasharray={`${dash} ${gap}`}
            style={{
              transform:'rotate(-90deg)',
              transformOrigin:'center',
              transition:'stroke-dasharray 0.7s cubic-bezier(0.4,0,0.2,1), stroke 0.4s',
              filter: `drop-shadow(0 0 4px ${stroke}55)`
            }}
          />
        </svg>
        {/* Centre label */}
        <div style={{
          position:'absolute', inset:0,
          display:'flex', alignItems:'center', justifyContent:'center'
        }}>
          <span className="font-mono font-bold"
                style={{ fontSize: 11, color: stroke, lineHeight: 1 }}>
            {Math.round(pct * 100)}<span style={{ fontSize: 8 }}>%</span>
          </span>
        </div>
      </div>
      <span className="font-mono uppercase tracking-wider"
            style={{ fontSize: 8, color: stroke, fontWeight: 700 }}>
        {label}
      </span>
      {sublabel && (
        <span className="font-mono" style={{ fontSize: 7.5, color: 'var(--muted)', textAlign:'center' }}>
          {sublabel}
        </span>
      )}
    </div>
  )
}

/* ── Module health row ───────────────────────────────── */
function ModuleRow({ name, ok, desc }) {
  return (
    <div style={{
      display:'flex', alignItems:'center', justifyContent:'space-between',
      padding:'5px 0', borderBottom:'1px solid rgba(30,58,95,0.25)'
    }}>
      <div>
        <span className="font-mono" style={{ fontSize: 10, color: 'var(--text)' }}>{name}</span>
        {desc && (
          <span className="font-mono" style={{ fontSize: 8, color: 'var(--muted)', marginLeft: 6 }}>{desc}</span>
        )}
      </div>
      <div style={{ display:'flex', alignItems:'center', gap: 5 }}>
        <span style={{
          width: 6, height: 6, borderRadius: '50%',
          background: ok ? 'var(--green)' : 'var(--red)',
          boxShadow: ok ? '0 0 7px var(--green)' : '0 0 7px var(--red)',
          display: 'inline-block',
          animation: ok ? 'ping-dot 2s ease infinite' : 'none'
        }}/>
        <span className="font-mono font-bold"
              style={{ fontSize: 8, color: ok ? 'var(--green)' : 'var(--red)' }}>
          {ok ? 'OK' : 'ERR'}
        </span>
      </div>
    </div>
  )
}

function formatBytes(n) {
  if (n >= 1048576) return (n / 1048576).toFixed(2) + ' MB'
  if (n >= 1024)    return (n / 1024).toFixed(1) + ' KB'
  return n + ' B'
}

/* ── Sparkline SVG ───────────────────────────────────── */
function Sparkline({ data, color }) {
  if (!data || data.length < 2) return null
  const W = 200, H = 26
  const max = Math.max(...data, 1)
  const pts = data.map((v, i) => {
    const x = (i / (data.length - 1)) * W
    const y = H - (v / max) * H
    return `${x.toFixed(1)},${y.toFixed(1)}`
  }).join(' ')
  const area = `0,${H} ` + pts + ` ${W},${H}`

  return (
    <svg width={W} height={H} viewBox={`0 0 ${W} ${H}`}
         style={{ width: '100%', height: H, overflow: 'visible' }}>
      <defs>
        <linearGradient id="spark-fill" x1="0" y1="0" x2="0" y2="1">
          <stop offset="0%"   stopColor={color} stopOpacity="0.2"/>
          <stop offset="100%" stopColor={color} stopOpacity="0"/>
        </linearGradient>
      </defs>
      <polygon points={area} fill="url(#spark-fill)"/>
      <polyline
        points={pts}
        fill="none"
        stroke={color}
        strokeWidth="1.5"
        strokeLinejoin="round"
        strokeLinecap="round"
      />
      {/* Last data point dot */}
      {(() => {
        const lastIdx = data.length - 1
        const lx = (lastIdx / (data.length - 1)) * W
        const ly = H - (data[lastIdx] / max) * H
        return <circle cx={lx} cy={ly} r="2.5" fill={color} style={{ filter:`drop-shadow(0 0 3px ${color})` }}/>
      })()}
    </svg>
  )
}

const MODULES = [
  { name: 'Shell',      desc: 'cmd loop'    },
  { name: 'FileSystem', desc: '64 inodes'   },
  { name: 'Scheduler',  desc: 'RR q=3'      },
  { name: 'MemManager', desc: '65536B VRAM' },
  { name: 'Parser',     desc: 'tokenizer'   },
  { name: 'Keyboard',   desc: 'stdin/read'  },
]

export default function SystemStats({ refresh }) {
  const [stats,   setStats]   = useState(null)
  const [loading, setLoading] = useState(true)
  const [error,   setError]   = useState(null)
  const [memHist, setMemHist] = useState([])

  const load = useCallback(async () => {
    try {
      const data = await fetchStatus()
      setStats(data)
      setError(null)
      setMemHist(h => [...h.slice(-14), data.memUsed ?? 0])
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

  const memTotal = stats?.memTotal ?? 65536
  const memUsed  = stats?.memUsed  ?? 0
  const memFree  = stats?.memFree  ?? memTotal
  const files    = stats?.files    ?? 0
  const procs    = stats?.procs    ?? 0

  const online = !error && stats !== null

  return (
    <div className="panel flex flex-col min-h-0">

      {/* Header */}
      <div className="panel-header">
        <div className="flex items-center gap-2">
          <div style={{
            width:22, height:22, borderRadius:6,
            background:'rgba(245,158,11,0.08)',
            border:'1px solid rgba(245,158,11,0.2)',
            display:'flex', alignItems:'center', justifyContent:'center'
          }}>
            <svg width="11" height="11" viewBox="0 0 24 24" fill="none"
                 stroke="var(--amber)" strokeWidth="2" strokeLinecap="round">
              <path d="M22 12h-4l-3 9L9 3l-3 9H2"/>
            </svg>
          </div>
          <span className="panel-title">System Stats</span>
        </div>
        <div className="flex items-center gap-2">
          {loading && <span className="font-mono text-[8px] animate-pulse" style={{ color:'var(--muted)' }}>updating…</span>}
          {error   && <span className="font-mono text-[8px]" style={{ color:'var(--red)' }}>● offline</span>}
          {!error && !loading && <span style={{ width:6, height:6, borderRadius:'50%', background:'var(--green)', boxShadow:'0 0 6px var(--green)', display:'inline-block', animation:'ping-dot 2s ease infinite' }}/>}
        </div>
      </div>

      <div className="flex-1 overflow-y-auto">

        {/* ── Gauge row ── */}
        <div style={{
          display:'flex', alignItems:'center', justifyContent:'space-around',
          padding:'10px 8px',
          borderBottom:'1px solid rgba(30,58,95,0.35)',
          gap: 6
        }}>
          <GaugeRing
            value={memUsed} max={memTotal}
            color="var(--cyan)"
            label="VRAM"
            sublabel={formatBytes(memUsed)}
          />
          <GaugeRing
            value={files} max={64}
            color="var(--purple)"
            label="Files"
            sublabel={`${files}/64`}
          />
          <GaugeRing
            value={procs} max={16}
            color="var(--green)"
            label="Procs"
            sublabel={`${procs}/16`}
          />
        </div>

        {/* ── Memory raw numbers ── */}
        <div style={{
          display:'grid', gridTemplateColumns:'1fr 1fr 1fr',
          borderBottom:'1px solid rgba(30,58,95,0.35)'
        }}>
          {[
            { label:'Total', val: formatBytes(memTotal), color:'var(--text-dim)' },
            { label:'Used',  val: formatBytes(memUsed),  color:'var(--amber)' },
            { label:'Free',  val: formatBytes(memFree),  color:'var(--green)' },
          ].map(({ label, val, color }) => (
            <div key={label} style={{
              display:'flex', flexDirection:'column', alignItems:'center',
              padding:'7px 4px',
              borderRight:'1px solid rgba(30,58,95,0.35)'
            }}>
              <span className="font-mono uppercase tracking-widest"
                    style={{ fontSize:7.5, color:'var(--muted)', marginBottom:2 }}>
                {label}
              </span>
              <span className="font-mono font-semibold"
                    style={{ fontSize:11, color }}>
                {val}
              </span>
            </div>
          ))}
        </div>

        {/* ── VRAM sparkline ── */}
        <div style={{
          padding:'8px 14px',
          borderBottom:'1px solid rgba(30,58,95,0.35)'
        }}>
          <div style={{ display:'flex', justifyContent:'space-between', marginBottom:6 }}>
            <span className="font-mono uppercase tracking-wider" style={{ fontSize:8, color:'var(--muted)' }}>
              VRAM History
            </span>
            <span className="font-mono" style={{ fontSize:8, color:'var(--cyan)' }}>
              {formatBytes(memFree)} free
            </span>
          </div>
          <Sparkline data={memHist} color="var(--cyan)"/>
        </div>

        {/* ── Module health ── */}
        <div style={{ padding:'8px 14px' }}>
          <span className="font-mono uppercase tracking-wider"
                style={{ fontSize:8, color:'var(--muted)', display:'block', marginBottom:6 }}>
            Module Health
          </span>
          {MODULES.map(m => (
            <ModuleRow key={m.name} name={m.name} desc={m.desc} ok={online}/>
          ))}
        </div>

      </div>
    </div>
  )
}
