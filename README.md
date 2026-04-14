# MiniOS — Phase 1

A minimal shell implemented in C using only raw system calls (`read`, `write`).  
No standard library I/O (`printf`, `scanf`) is used anywhere.

---

## Folder Structure

```
Mini-OS-Project/
├── src/
│   ├── main.c        # Entry point
│   ├── shell.c/.h    # Shell REPL loop + command dispatch
│   ├── parser.c/.h   # Tokenises raw input into Command struct
│   ├── string.c/.h   # Custom string functions (no <string.h>)
│   ├── math.c/.h     # Basic arithmetic (multiply, divide)
│   ├── screen.c/.h   # Output via write()
│   └── keyboard.c/.h # Input via read()
├── Makefile
└── README.md
```

---

## Modules

### Shell (`shell.c`)
- Displays a `miniOS> ` prompt
- Reads a line with `kb_read_line()`
- Parses and dispatches to the correct handler
- Supported commands: `help`, `echo`, `math`, `clear`, `exit`

### Parser (`parser.c`)
- Splits raw input string into a `Command` struct
- `cmd` holds the command name; `args[]` holds arguments
- Uses the custom `my_tokenize()` from `string.c`

### String (`string.c`)
- `my_strlen()` — string length
- `my_strcpy()` — string copy
- `my_strcmp()` — string compare
- `my_tokenize()` — split by delimiter
- `my_int_to_str()` / `my_str_to_int()` — number conversion

### Math (`math.c`)
- `my_multiply(a, b)` — repeated addition
- `my_divide(a, b)` — repeated subtraction
- Add and subtract use native `+` / `-` operators in the shell

### Screen (`screen.c`)
- `screen_print(s)` — writes string to stdout via `write()`
- `screen_print_line(s)` — prints with trailing newline
- `screen_newline()` — prints a blank line

### Keyboard (`keyboard.c`)
- `kb_read_line(buf)` — reads one line via `read()`, strips newline

---

## Build & Run

```bash
make        # compile
make run    # compile and run
make clean  # remove binaries
```

---

## Commands

| Command              | Description                        |
|----------------------|------------------------------------|
| `help`               | Show available commands            |
| `echo <text>`        | Print text to screen               |
| `math <a> <op> <b>` | Arithmetic — op: `+` `-` `*` `/`  |
| `clear`              | Clear the terminal                 |
| `exit`               | Quit MiniOS                        |

### Example

```
miniOS> echo Hello World
$ echo Hello World
Hello World

miniOS> math 10 + 3
$ math 10 + 3
10 + 3 = 13

miniOS> math 15 / 4
$ math 15 / 4
15 / 4 = 3

miniOS> help
$ help
MiniOS Phase 1  --  Available Commands
...
```