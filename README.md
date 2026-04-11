# MiniOS — Phase 1

**Team:** TeamHamza  
**Members:** Abhishek (230137) · Prerak (230039)  
**Track:** Mini Operating System

---

## What is MiniOS?

A lightweight command-line OS simulation written in **pure C**, built entirely from scratch **without** any standard libraries (`string.h`, `math.h`, or custom `malloc`).

---

## Custom Libraries Built

| Library | Purpose |
|---------|---------|
| `math.c` | multiply, divide, modulo, abs, max, min, clamp |
| `string.c` | strlen, strcpy, strcmp, strcat, tokenize, int↔str |
| `memory.c` | 64KB virtual RAM array + alloc() / dealloc() |
| `screen.c` | clear, cursor move, print string/int/char |
| `keyboard.c` | readLine() + non-blocking keyPressed() |

---

## How to Build & Run

```bash
make
./miniOS