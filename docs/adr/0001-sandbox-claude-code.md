# 0001 Run Claude Code in a filesystem and network sandbox

Status: Accepted, pending verification (see [SANDBOX-TEST](../SANDBOX-TEST.md))

## Context
Claude Code runs shell commands on a machine with personal data in `~`. The project only needs its own directory, a few tool caches and a handful of download hosts.

## Decision
Enable the sandbox in `.claude/settings.json`. Deny reading `~/`, then allow only the project, `/tmp` and the tool directories. Allow network access only to named domains. Hardware and credentialed commands (flashing, serial, `gh`, git remote operations) are excluded from the sandbox and still go through permission prompts.

Read-only where possible: `~/.sdkman`, `~/jbinst` and the git config are readable but not writable, because shell init may source them and write access would be a route out of the sandbox.

## Consequences
- A new tool or host needs an explicit entry. A blocked request is the signal to add one.
- The exact lists live in `.claude/settings.json`. This record holds the reasoning only.
- Leading-wildcard patterns in `excludedCommands` are unverified.
