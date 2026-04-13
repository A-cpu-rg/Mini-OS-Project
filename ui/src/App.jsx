import { useState, useCallback, useEffect } from 'react'
import Terminal         from './components/Terminal'
import FileSystemPanel  from './components/FileSystemPanel'
import ProcessMonitor   from './components/ProcessMonitor'
import SystemStats      from './components/SystemStats'
import { checkHealth }  from './api/client'

const MODULES = ['Shell', 'VirtualFS', 'Scheduler', 'MemMgr', 'Parser', 'Keyboard']

function fmtUptime(s) {
  const h   = Math.floor(s / 3600)
  const m   = Math.floor((s % 3600) / 60)
  const sec = s % 60
  return `${String(h).padStart(2,'0')}:${String(m).padStart(2,'0')}:${String(sec).padStart(2,'0')}`
}

export default function App() {
  const [tick,   setTick]   = useState(0)
  const [online, setOnline] = useState(null)   // null=checking
  const [uptime, setUptime] = useState(0)

  const handleCommandRan = useCallback(() => setTick(t => t + 1), [])

  /* Health-check every 5s */
  useEffect(() => {
    const ping = async () => {
      try {
        const { ready } = await checkHealth()
        setOnline(!!ready)
      } catch {
        setOnline(false)
      }
    }
    ping()
    const id = setInterval(ping, 5000)
    return () => clearInterval(id)
  }, [])

  /* Uptime counter */
  useEffect(() => {
    const id = setInterval(() => setUptime(u => u + 1), 1000)
    return () => clearInterval(id)
  }, [])

  return (
    <div className="flex flex-col h-screen bg-grid overflow-hidden"
         style={{ position:'relative', zIndex:1 }}>

      {/* ── Ambient light blobs ─────────────────────────── */}
      <div aria-hidden="true" style={{
        position:'fixed', top:'-15%', left:'-8%',
        width:'50vw', height:'50vw', borderRadius:'50%',
        background:'radial-gradient(circle, rgba(0,212,255,0.028) 0%, transparent 65%)',
        pointerEvents:'none', zIndex:0
      }}/>
      <div aria-hidden="true" style={{
        position:'fixed', bottom:'-12%', right:'-8%',
        width:'45vw', height:'45vw', borderRadius:'50%',
        background:'radial-gradient(circle, rgba(168,85,247,0.025) 0%, transparent 65%)',
        pointerEvents:'none', zIndex:0
      }}/>
      <div aria-hidden="true" style={{
        position:'fixed', top:'40%', left:'50%',
        width:'30vw', height:'30vw', borderRadius:'50%',
        transform:'translate(-50%,-50%)',
        background:'radial-gradient(circle, rgba(0,255,157,0.012) 0%, transparent 65%)',
        pointerEvents:'none', zIndex:0
      }}/>

      {/* ── Header ─────────────────────────────────────── */}
      <header className="flex items-center justify-between px-5 py-2.5"
              style={{
                background:'rgba(4,8,16,0.96)',
                borderBottom:'1px solid rgba(30,58,95,0.8)',
                backdropFilter:'blur(20px)',
                boxShadow:'0 4px 32px rgba(0,0,0,0.7), 0 1px 0 rgba(0,212,255,0.04)',
                position:'relative', zIndex:10, flexShrink:0
              }}>

        {/* Branding */}
        <div className="flex items-center gap-3">
          <div style={{
            width:36, height:36, borderRadius:9,
            background:'linear-gradient(135deg, rgba(0,212,255,0.12) 0%, rgba(168,85,247,0.08) 100%)',
            border:'1px solid rgba(0,212,255,0.22)',
            display:'flex', alignItems:'center', justifyContent:'center',
            boxShadow:'0 0 20px rgba(0,212,255,0.12)',
            animation:'float 4s ease-in-out infinite'
          }}>
            <svg width="18" height="18" viewBox="0 0 24 24" fill="none"
                 stroke="var(--cyan)" strokeWidth="1.6" strokeLinecap="round">
              <rect x="2" y="3" width="20" height="14" rx="2"/>
              <path d="M8 21h8M12 17v4M5 7l3 3-3 3"/>
              <path d="M11 13h5"/>
            </svg>
          </div>

          <div>
            <h1 className="font-mono font-bold glow-cyan"
                style={{ fontSize:15, letterSpacing:'0.22em', color:'white', lineHeight:1.1 }}>
              MINI<span style={{ color:'var(--cyan)' }}>OS</span>
            </h1>
            <p className="font-mono" style={{ fontSize:8, letterSpacing:'0.18em', color:'var(--muted)', lineHeight:1.2 }}>
              v2.0 · C FROM SCRATCH · NO STDLIB
            </p>
          </div>
        </div>

        {/* Module badges — hidden on small screens */}
        <div className="hidden lg:flex items-center gap-1.5">
          {MODULES.map(m => (
            <span key={m} className="module-chip">
              <span style={{ width:4, height:4, borderRadius:'50%', background:'var(--green)',
                             display:'inline-block', boxShadow:'0 0 5px var(--green)', flexShrink:0 }}/>
              {m}
            </span>
          ))}
        </div>

        {/* Right: uptime + arch + status */}
        <div className="flex items-center gap-4">
          <div className="hidden sm:flex flex-col items-end">
            <span className="font-mono uppercase tracking-widest" style={{ fontSize:8, color:'var(--muted)' }}>Uptime</span>
            <span className="font-mono font-semibold glow-amber" style={{ fontSize:12, color:'var(--amber)' }}>
              {fmtUptime(uptime)}
            </span>
          </div>

          <div className="hidden sm:flex flex-col items-end">
            <span className="font-mono uppercase tracking-widest" style={{ fontSize:8, color:'var(--muted)' }}>Arch</span>
            <span className="font-mono font-semibold" style={{ fontSize:11, color:'var(--cyan)' }}>x86-sim</span>
          </div>

          {/* Status pill */}
          <div style={{
            display:'flex', alignItems:'center', gap:6,
            padding:'5px 12px', borderRadius:8,
            background: online === false ? 'rgba(255,68,102,0.07)' : 'rgba(0,255,157,0.06)',
            border: `1px solid ${online === false ? 'rgba(255,68,102,0.25)' : 'rgba(0,255,157,0.2)'}`,
          }}>
            {online === null
              ? <span className="font-mono animate-pulse" style={{ fontSize:9, color:'var(--amber)', letterSpacing:'0.12em' }}>CONNECTING…</span>
              : online
              ? <>
                  <span className="online-dot"/>
                  <span className="font-mono font-bold" style={{ fontSize:10, color:'var(--green)', letterSpacing:'0.12em' }}>RUNNING</span>
                </>
              : <>
                  <span className="offline-dot"/>
                  <span className="font-mono font-bold" style={{ fontSize:10, color:'var(--red)', letterSpacing:'0.12em' }}>OFFLINE</span>
                </>
            }
          </div>
        </div>
      </header>

      {/* ── Main layout ────────────────────────────────── */}
      <main className="flex flex-1 min-h-0 overflow-hidden"
            style={{ gap:10, padding:10, position:'relative', zIndex:1 }}>

        {/* Terminal — 58% */}
        <div className="flex flex-col min-h-0" style={{ flex:'0 0 58%' }}>
          <Terminal onCommandRan={handleCommandRan}/>
        </div>

        {/* Side panels — 42% */}
        <div className="flex flex-col min-h-0 flex-1 overflow-hidden" style={{ gap:10 }}>

          {/* File System — 30% */}
          <div className="flex flex-col min-h-0" style={{ flex:'0 0 29%' }}>
            <FileSystemPanel refresh={tick}/>
          </div>

          {/* Process Monitor — 35% */}
          <div className="flex flex-col min-h-0" style={{ flex:'0 0 35%' }}>
            <ProcessMonitor refresh={tick}/>
          </div>

          {/* System Stats — rest */}
          <div className="flex flex-col min-h-0" style={{ flex:'1 1 0' }}>
            <SystemStats refresh={tick}/>
          </div>

        </div>
      </main>

      {/* ── Footer ─────────────────────────────────────── */}
      <footer style={{
        flexShrink:0, display:'flex', alignItems:'center',
        justifyContent:'space-between', padding:'5px 20px',
        background:'rgba(4,8,16,0.96)',
        borderTop:'1px solid rgba(30,58,95,0.6)',
        position:'relative', zIndex:10
      }}>
        <span className="font-mono" style={{ fontSize:8, color:'var(--muted)', letterSpacing:'0.08em' }}>
          MiniOS Dashboard · React + Vite + Tailwind + C
        </span>
        <div className="flex items-center gap-4">
          <span className="font-mono" style={{ fontSize:8, color:'var(--muted)' }}>
            Bridge → :3001
          </span>
          <span className="font-mono" style={{ fontSize:8, color:'var(--muted)' }}>
            UI → :5173
          </span>
          <span className="font-mono" style={{ fontSize:8, color:'var(--muted)' }}>
            {new Date().toLocaleTimeString()}
          </span>
        </div>
      </footer>

    </div>
  )
}
