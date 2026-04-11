# MiniOS — Phase 1

**Team:** TeamHamza  
**Members:** Abhishek (230137), Prerak (230039)  
**Track:** Mini Operating System (Track B)

---

## Overview

MiniOS is a lightweight command-line operating system simulation built entirely in **C from scratch**, without using standard libraries such as `<string.h>`, `<math.h>`, or dynamic memory functions like `malloc()`.

The project demonstrates core **systems programming concepts**, including:
- Custom memory management
- String processing
- Command parsing
- Terminal interaction
- Modular software design

---

## Features (Phase 1)

- Interactive shell with command prompt
- Custom-built standard libraries
- Dynamic memory allocation using virtual RAM
- Command parsing using tokenizer
- Real-time command execution
- Memory leak detection system

---

## Custom Libraries

| Library | Description |
|--------|-------------|
| `math.c` | Implements arithmetic operations: multiply, divide, modulo, max, min |
| `string.c` | String utilities: length, copy, compare, tokenize, conversions |
| `memory.c` | 64KB virtual RAM with custom `alloc()` and `dealloc()` |
| `screen.c` | Terminal control: clear screen, print text, cursor movement |
| `keyboard.c` | Input handling: line input and non-blocking key detection |

---

## Build & Run

```bash
make
./miniOS