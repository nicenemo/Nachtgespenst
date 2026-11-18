# 0003 Pin the JDK per script with `//JAVA`

Status: Accepted

## Context
The generator uses preview features, so it needs a specific recent JDK. The system Java may be newer or older.

## Decision
Each `tools/*.jsh` script declares `//JAVA <n>` and `//JBANG_JSHELL_OPTIONS --enable-preview`. JBang downloads that JDK into `~/.jbang/cache/jdks` on demand. Only GA releases are pinned, never `-ea` builds.

## Consequences
- A Java upgrade is a one-line change per script. No sandbox change is needed, because the exemptions name no version.
- Current pin: 27. Java 28 is early-access only (checked 2026-11-17).
