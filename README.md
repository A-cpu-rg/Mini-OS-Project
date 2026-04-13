<div align="center">

# 🖥️ MiniOS

### A Complete Mini Operating System Built in C From Scratch

[![C](https://img.shields.io/badge/Core-C11-00d4ff?style=flat-square&logo=c)](#)
[![Node.js](https://img.shields.io/badge/Bridge-Node.js-00ff9d?style=flat-square&logo=nodedotjs)](#)
[![React](https://img.shields.io/badge/UI-React%20+%20Vite-a855f7?style=flat-square&logo=react)](#)
[![Tailwind](https://img.shields.io/badge/Styles-Tailwind%20CSS-fbbf24?style=flat-square&logo=tailwindcss)](#)
[![License](https://img.shields.io/badge/License-MIT-ff4466?style=flat-square)](#)

> **No stdlib. No malloc. No printf. Pure systems programming.**
> 
> MiniOS is a fully modular, C-built mini operating system simulator with a custom shell,
> virtual file system, memory manager, and process scheduler — all wired to a high-end
> React dashboard via a Node.js bridge.

</div>

---

## 📸 Screenshots

> *[ Terminal Panel — Interactive OS Shell ]*

> *[ File System Viewer — Virtual FS with inode table ]*

> *[ Process Monitor — Round-Robin Scheduler ]*

> *[ System Stats — VRAM Gauges + Sparkline ]*

---

## ✨ Features

| Module | Description |
|---|---|
| 🐚 **Custom Shell** | Interactive REPL with command history, Tab-completion, Ctrl+L clear |
| 📁 **Virtual File System** | 64-file inode table, create/read/write/delete, persistent in VRAM |
| ⚙️ **Process Scheduler** | Round-Robin queue (quantum=3), multi-state PCBs (READY/RUNNING/BLOCKED/DONE) |
| 🧠 **Memory Manager** | 65536-byte VRAM pool, first-fit allocator, block coalescing, zero-on-free |
| 🔤 **Custom String Library** | strlen, strcpy, strcmp, strcat, tokenize, int↔str — no `<string.h>` |
| 🔢 **Math Module** | multiply, divide, modulo, max, min — no `<math.h>` |
| 📺 **Screen Module** | write(1,…)-based output — no `printf` |
| ⌨️ **Keyboard Module** | read(0,…)-based input — no `scanf` |
| 🌉 **Node.js Bridge** | Persistent child_process with sentinel-based framing, serial command queue |
| 🎨 **React Dashboard** | Glassmorphism UI, animated gauges, live sparkline, process kill buttons |

---

## 🏗️ Architecture

```
Mini-OS-Project/
├── src/                       ← C kernel modules
│   ├── main.c                 ← Entry point (calls shell_init + run_shell)
│   ├── shell.c / .h           ← Interactive REPL + command dispatcher
│   ├── fs.c / .h              ← Virtual File System (64-inode table)
│   ├── scheduler.c / .h       ← Round-Robin process scheduler (PCB queue)
│   ├── memory.c / .h          ← First-fit VRAM allocator (65536 B)
│   ├── parser.c / .h          ← Command tokeniser (no strtok)
│   ├── string.c / .h          ← Custom string library (no <string.h>)
│   ├── math.c / .h            ← Arithmetic helpers (no <math.h>)
│   ├── screen.c / .h          ← write(1,…) output helpers
│   └── keyboard.c / .h        ← read(0,…) input helpers
│
├── server/
│   └── server.js              ← Node.js bridge (Express + child_process)
│
├── ui/                        ← React + Vite + Tailwind frontend
│   ├── src/
│   │   ├── App.jsx            ← Root layout + health polling + uptime
│   │   ├── index.css          ← Design system (glassmorphism, keyframes)
│   │   ├── main.jsx           ← Vite entry
│   │   ├── api/client.js      ← Axios API client
│   │   └── components/
│   │       ├── Terminal.jsx       ← Interactive shell panel
│   │       ├── FileSystemPanel.jsx ← VFS viewer
│   │       ├── ProcessMonitor.jsx  ← Scheduler viewer + kill buttons
│   │       └── SystemStats.jsx    ← VRAM gauges + sparkline + module health
│   └── vite.config.js         ← Dev server + /api proxy → :3001
│
├── Makefile                   ← gcc build system
├── package.json               ← Monorepo scripts (build:c, server, ui, dev)
└── miniOS                     ← Compiled binary (auto-generated)
```

### Communication Flow

```
Browser (React)
    │  HTTP POST /api/command { command: "ls" }
    ▼
Node.js Bridge (:3001)
    │  child.stdin.write("ls\n")
    ▼
C Binary (miniOS) ← persistent child_process
    │  stdout → "file.txt (12 B)\n__END__\n"
    ▼
Node.js Bridge (framing by __END__ sentinel)
    │  HTTP Response { output: "file.txt (12 B)" }
    ▼
Browser → updates Terminal + side panels
```

---

## 🔧 C Modules Deep Dive

### Memory Manager (`memory.c`)
- **VRAM**: Static `char vram[65536]` — zero-initialised on boot
- **Block table**: Array of `{ start, size, used }` descriptors
- **First-fit**: Scans freed blocks before carving new space
- **my_alloc(n)** → returns pointer into VRAM or NULL if full
- **my_dealloc(p)** → zero-fills and marks block free

### Virtual File System (`fs.c`)
- **Inode table**: `FSEntry table[64]` — each has `name[32]`, `data[512]`, `size`, `used`
- **Operations**: `fs_create`, `fs_write`, `fs_read`, `fs_delete`, `fs_ls`, `fs_count`
- No disk I/O — everything lives in process memory

### Process Scheduler (`scheduler.c`)
- **PCB**: `{ pid, name, burst, remaining, priority, state, used }`
- **Algorithm**: Round-Robin with time quantum = 3
- **States**: READY → RUNNING → DONE (or BLOCKED)
- **sched_tick()**: One full pass over the queue — advances each READY process by `min(remaining, quantum)`

### Command Parser (`parser.c`)
- **my_tokenize()**: Splits input on spaces, replacing delimiter with `\0`
- Returns a `Command { cmd[32], args[16][32], argc }` struct
- Strips trailing `\r\n\t`

---

## 🚀 How to Run

### Prerequisites
```bash
gcc --version     # GCC 11+ recommended
node --version    # Node.js 18+
npm --version     # npm 9+
```

### 1. Clone & Install
```bash
git clone https://github.com/your-username/Mini-OS-Project.git
cd Mini-OS-Project

# Install root concurrently
npm install

# Install server deps
cd server && npm install && cd ..

# Install UI deps
cd ui && npm install && cd ..
```

### 2. Build C Binary
```bash
make
# Produces ./miniOS binary
```

### 3. Start Everything (one command)
```bash
npm run dev
# Starts: C build + Node.js server (:3001) + React UI (:5173)
```

### Or start individually
```bash
# Terminal 1 — Node.js bridge
node server/server.js

# Terminal 2 — React UI
cd ui && npm run dev
```

### 4. Open Dashboard
```
http://localhost:5173
```

---

## 💻 Shell Commands

| Command | Syntax | Description |
|---|---|---|
| `help` | `help` | Show all commands |
| `create` | `create <file>` | Create a new file in VFS |
| `write` | `write <file> <data...>` | Write/overwrite file content |
| `read` | `read <file>` | Print file content |
| `ls` | `ls` | List all files with sizes |
| `delete` | `delete <file>` | Delete a file |
| `run` | `run <name> <burst> <priority>` | Add process to scheduler |
| `ps` | `ps` | List all active processes |
| `tick` | `tick` | Execute one scheduler cycle |
| `kill` | `kill <pid>` | Terminate process by PID |
| `mem` | `mem` | Show VRAM allocation stats |
| `status` | `status` | Full system status snapshot |
| `math` | `math <a> <op> <b>` | Arithmetic: `+ - * / % max min` |
| `echo` | `echo <text...>` | Print text to terminal |
| `clear` | `clear` | Clear terminal output |
| `exit` | `exit` | Shutdown MiniOS |

### Demo Session
```bash
# Create and write a file
create notes.txt
write notes.txt Hello World this is MiniOS
read notes.txt

# Spawn processes
run kernel 15 8
run init 10 5
run bash 6 3
ps

# Advance scheduler
tick
tick
ps

# Check memory
mem
status
```

---

## 🔌 REST API

| Method | Endpoint | Description |
|---|---|---|
| `POST` | `/api/command` | Run any shell command |
| `GET` | `/api/files` | Get parsed file list |
| `GET` | `/api/processes` | Get parsed process list |
| `GET` | `/api/memory` | Get VRAM stats |
| `GET` | `/api/status` | Get full system status |
| `GET` | `/api/all` | Aggregate (files+procs+status) |
| `GET` | `/api/health` | Bridge health check |

---

## 🎨 UI Design

- **Theme**: Deep space dark mode (`#040810` base)
- **Style**: Glassmorphism panels with backdrop-blur
- **Typography**: Inter (UI) + JetBrains Mono (terminal/code)
- **Animations**: Slide-in lines, gauge transitions, sparkline, ambient glow blobs
- **Terminal**: CRT scanlines, syntax-coloured commands, Tab autocomplete, ↑↓ history
- **Live reload**: All side panels auto-poll every 3s

---

## 🧪 What Makes This a Real OS Simulator

1. **No `stdlib.h`** — no `malloc`, `free`, `printf`, `scanf`, `string.h`
2. **Custom allocator** — first-fit VRAM with block table
3. **Custom I/O** — `write(1, …)` / `read(0, …)` syscalls only
4. **Custom string ops** — all implemented from scratch
5. **Modular kernel** — each subsystem has its own `.c` + `.h`
6. **Bridge protocol** — sentinel-framed IPC between C process and Node.js
7. **Multi-state PCBs** — proper process control blocks with lifecycle

---

## 📄 License

MIT — see [LICENSE](LICENSE) for details.

---

<div align="center">

Built with ❤️ in C, Node.js, and React

</div>