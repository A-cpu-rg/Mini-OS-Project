#!/bin/sh

set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT_DIR"

make

OUT_FILE=$(mktemp)
trap 'rm -f "$OUT_FILE"' EXIT

printf 'help\ntouch demo.txt\nwrite demo.txt hello\nread demo.txt\nls\ndelete demo.txt\nmem\nexit\n' | ./miniOS >"$OUT_FILE"

grep -F "MiniOS v2.0 Loaded" "$OUT_FILE" >/dev/null
grep -F "touch <file>" "$OUT_FILE" >/dev/null
grep -F "[OK] File 'demo.txt' created" "$OUT_FILE" >/dev/null
grep -F "[OK] Data written to 'demo.txt'" "$OUT_FILE" >/dev/null
grep -F "hello" "$OUT_FILE" >/dev/null
grep -F "[OK] File 'demo.txt' deleted" "$OUT_FILE" >/dev/null
grep -F "Memory Status" "$OUT_FILE" >/dev/null
grep -F "[OK] Goodbye!" "$OUT_FILE" >/dev/null

printf 'Smoke test passed.\n'
