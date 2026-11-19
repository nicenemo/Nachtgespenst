#!/usr/bin/env python3
"""Brings the Claude Code skills, agents, rules and commands in .claude/ up to date with the shell of the boards (docs/wiki/shell.md).

Those folders are write-protected for Claude Code, so this is run by hand, from the repository root:

    python3 tools/fix-claude-docs.py            # shows what would change
    python3 tools/fix-claude-docs.py --write    # changes the files

What it does: the serial commands in backticks and in the quoted arguments of `jbang tools/Serial.java S3 "..."` become the lower-case commands
without "?" (`INFO` -> `info`, `LAYER 26` -> `layer 26`, `STATE?` -> `state`, `PREFER USB|BLE` -> `prefer usb|ble`), and the test counts
of SerialTest are brought up to date. Running it twice changes nothing the second time.
"""
import glob, re, sys

CMDS = {'PING', 'INFO', 'STATE', 'LAYER', 'KEY', 'TAP', 'SHOT', 'PERF', 'WAKE', 'BL', 'LEDS', 'SET', 'PREFER', 'ONLY',
        'LAMPS', 'DISK', 'SD', 'MIC', 'BAR', 'WAVE', 'TELEM'}
# text that is not a command but has to follow the tests: (old, new)
TEXT = [("S3: 29 checks, C2: 11", "S3: 48 checks, C2: 30")]


def conv(lit):
    """One command line (without the quotes): the command word in lower case without '?', the upper-case keywords after it in lower case."""
    m = re.match(r'^([A-Z]+)(\??)(?=$|[ ,;:)])(.*)$', lit, re.S)
    if not m or m.group(1) not in CMDS:
        return lit
    rest = re.sub(r'\b[A-Z]{2,}\b', lambda t: t.group(0).lower(), m.group(3))
    return m.group(1).lower() + rest


def fix(text):
    def span(m):
        return '`' + conv(m.group(1)) + '`'
    def quoted(m):      # jbang tools/Serial.java S3 "INFO" "STATE?" ...
        args = re.sub(r'"([^"\n]*)"', lambda a: '"' + conv(a.group(1)) + '"', m.group(2))
        return m.group(1) + args
    text = re.sub(r'`([^`\n]+)`', span, text)
    text = re.sub(r'(Serial\.java\s+(?:S3|C2|<S3\|C2>|core2|cores3)\s+)((?:"[^"\n]*"\s*)+)', quoted, text)
    for old, new in TEXT:
        text = text.replace(old, new)
    return text


def main():
    write = '--write' in sys.argv
    files = sorted(set(glob.glob('.claude/**/*.md', recursive=True)))
    changed = 0
    for f in files:
        old = open(f, encoding='utf-8').read()
        new = fix(old)
        if new == old:
            continue
        changed += 1
        print(('changed ' if write else 'would change ') + f)
        for a, b in zip(old.splitlines(), new.splitlines()):
            if a != b:
                print('  - ' + a.strip()[:140]); print('  + ' + b.strip()[:140])
        if write:
            open(f, 'w', encoding='utf-8').write(new)
    print(f"{changed} file(s) {'changed' if write else 'would change; run with --write to apply'}")


if __name__ == '__main__':
    main()
