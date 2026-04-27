#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path


def strip_comments_preserve_layout(src: str) -> str:
    OUT: list[str] = []
    i = 0
    n = len(src)

    in_sl_comment = False
    in_ml_comment = False
    in_str = False
    str_quote = ""
    esc = False

    while i < n:
        ch = src[i]
        nxt = src[i + 1] if i + 1 < n else ""

        if in_sl_comment:
            if ch == "\n":
                in_sl_comment = False
                OUT.append("\n")
            else:
                OUT.append(" ")
            i += 1
            continue

        if in_ml_comment:
            if ch == "*" and nxt == "/":
                OUT.append(" ")
                OUT.append(" ")
                in_ml_comment = False
                i += 2
                continue
            if ch == "\n":
                OUT.append("\n")
            else:
                OUT.append(" ")
            i += 1
            continue

        if in_str:
            OUT.append(ch)
            if esc:
                esc = False
            else:
                if ch == "\\":
                    esc = True
                elif ch == str_quote:
                    in_str = False
                    str_quote = ""
            i += 1
            continue

        if ch == '"' or ch == "'":
            in_str = True
            str_quote = ch
            OUT.append(ch)
            i += 1
            continue

        if ch == "/" and nxt == "/":
            in_sl_comment = True
            OUT.append(" ")
            OUT.append(" ")
            i += 2
            continue

        if ch == "/" and nxt == "*":
            in_ml_comment = True
            OUT.append(" ")
            OUT.append(" ")
            i += 2
            continue

        OUT.append(ch)
        i += 1

    return "".join(OUT)


def process_file(path: Path) -> bool:
    original = path.read_text(encoding="utf-8")
    cleaned = strip_comments_preserve_layout(original)
    if cleaned == original:
        return False
    path.write_text(cleaned, encoding="utf-8")
    return True


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("paths", nargs="+", help="Files to process")
    args = ap.parse_args()

    changed = 0
    for p in args.paths:
        path = Path(p)
        if not path.is_file():
            raise SystemExit(f"Not a file: {p}")
        if process_file(path):
            changed += 1

    print(f"updated_files={changed}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
