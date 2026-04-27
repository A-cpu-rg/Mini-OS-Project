# MiniOS — Custom Terminal-Based Operating System in C

## Overview
MiniOS is a small terminal-based “operating system” written in C that implements a custom shell and core OS-like subsystems without relying on standard library I/O. Phase 2 extends the system with a virtual memory allocator, an in-memory virtual file system, a cooperative task scheduler, and a structured terminal UI for clearer output and debugging.

## Features
- **Custom Shell (non-blocking)**: command loop with structured parsing and clean command dispatch
- **Virtual Memory System**: 1MB RAM arena with a custom allocator (no `malloc/free`)
- **Virtual File System**: in-memory files with `touch`, `write`, `read`, `ls`
- **Task Scheduler**: cooperative background counter task (demonstration “tick” via `run`)
- **Structured Terminal UI**: formatted output with status tags like `[OK]` / `[ERROR]`
- **Memory Stats**: `mem` command to inspect allocator usage

## Architecture
MiniOS is split into small modules, each isolated behind a header:

- **`shell.c`**: REPL loop, command routing, user-facing UX
- **`parser.c`**: tokenization + `Command` structure construction
- **`string.c`**: custom string utilities (no `<string.h>`)
- **`math.c`**: arithmetic helpers used by the `math` command
- **`memory.c`**: 1MB memory arena + allocator + memory statistics
- **`filesystem.c`**: virtual in-memory files and basic file operations
- **`scheduler.c`**: cooperative scheduler + background demo task(s)
- **`screen.c`**: all terminal output via `write()`, including UI helpers
- **`keyboard.c`**: input via `read()`

## Commands
| Command | Description |
|---|---|
| `help` | Show available commands |
| `echo <text...>` | Print text |
| `math <a> <op> <b>` | Arithmetic (`+` `-` `*` `/`) |
| `clear` | Clear screen + show boot banner |
| `exit` | Quit MiniOS |
| `touch <file>` | Create file (if missing) |
| `write <file> <data...>` | Write data to a file (overwrites) |
| `read <file>` | Read file contents |
| `ls` | List files (with sizes) |
| `run` | Run one scheduler tick (demo) |
| `mem` | Show memory/allocator status |

## Sample Usage
```
## MiniOS v2.0 Loaded
--------------------------------
Type 'help' to begin

miniOS:/home$ help
[OK] MiniOS v2.0 — Commands
...

miniOS:/home$ touch notes.txt
[OK] File 'notes.txt' created

miniOS:/home$ write notes.txt Hello from MiniOS
[OK] Data written to 'notes.txt'

miniOS:/home$ read notes.txt
----- FILE: notes.txt -----
Hello from MiniOS

miniOS:/home$ ls
[1] notes.txt    (17 bytes)

miniOS:/home$ run
[Scheduler] Counter: 120

miniOS:/home$ mem
[OK] Memory Status
...
```

## Build & Run
```bash
make
make run
make clean
```

## Constraints
- No `<string.h>`, `<math.h>`
- No `malloc/free`
- I/O is done via raw system calls (`read`, `write`)

## Known Limitations
- No persistent storage (VFS is in-memory only)
- Cooperative scheduling (not preemptive)
- Limited RAM (1MB arena)
