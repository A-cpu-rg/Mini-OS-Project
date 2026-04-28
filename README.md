# MiniOS - Custom Terminal Operating System in C

## Overview
MiniOS is a small Track B project: a terminal-based mini operating system written in C. It uses five custom libraries for string handling, math, memory, screen output, and keyboard input, then combines them into a shell with a virtual file system and a cooperative scheduler.

## What it demonstrates
- Custom `string`, `math`, `memory`, `screen`, and `keyboard` libraries
- A non-blocking shell loop
- In-memory virtual files backed by the custom allocator
- A background scheduler task that keeps running while the shell stays usable
- Terminal output and input handled without `string.h`, `math.h`, `malloc`, or `free`

## How it works
The flow is simple and easy to explain in evaluation:
1. Keyboard reads user input.
2. Parser splits the line into command and arguments.
3. Shell decides which command to run.
4. Memory allocates or frees file data.
5. Math helps with arithmetic and boundary checks.
6. Screen prints the final result to the terminal.

## Commands
| Command | Description |
|---|---|
| `help` | Show available commands |
| `echo <text...>` | Print text |
| `math <a> <op> <b>` | Arithmetic using `+`, `-`, `*`, `/` |
| `touch <file>` | Create a file in virtual memory |
| `write <file> <data...>` | Write or overwrite file contents |
| `read <file>` | Read a file from virtual memory |
| `ls` | List all files with sizes |
| `delete <file>` | Delete a file and free its memory |
| `run` | Run one scheduler tick |
| `mem` | Show virtual memory usage |
| `clear` | Clear the terminal and restart the banner |
| `exit` | Quit MiniOS |

## Build and Run
```bash
make
./miniOS
```

Optional cleanup:
```bash
make clean
```

## Submission Notes
- This project follows Track B: Mini Operating System.
- Files are stored only in RAM, so they reset when the program closes.
- The scheduler is cooperative, not preemptive.
- The virtual RAM size is 1 MB.

## Known Issues
- No persistent storage yet.
- No real multitasking or process isolation.
- Input handling is terminal-based, so it depends on a POSIX-like environment.

## Demo Checklist
- `help` shows the full command list
- `touch`, `write`, `read`, `ls`, and `delete` work together
- `run` shows the background counter
- `mem` shows allocator usage
- `clear` redraws the boot banner cleanly


