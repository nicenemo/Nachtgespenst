# 0016 Scan for secrets with trufflehog in git hooks

Status: Accepted 2026-11-18.

## Context
The repository is public and the project tooling talks to hardware, a webcam and GitHub. A secret that is pushed has to be treated as leaked, even if the commit is removed later. The history and the working tree were scanned with trufflehog 3.99.9 on 2026-11-18 and are clean (the one unverified hit is a URI example in a FastLED test file under the gitignored `tools/ledtest/.pio/`).

## Decision
- Two tracked hooks in `.githooks/`, switched on per checkout with `git config core.hooksPath .githooks`:
  - `pre-commit` scans the staged content (exported with `git checkout-index` to a temporary folder) with `trufflehog filesystem --no-verification`. It needs no network, so it also works inside the Claude Code sandbox.
  - `pre-push` scans the commits that are about to be pushed (`--since-commit @{upstream}`, or everything for a new branch), with verification. A finding in any state (verified, unverified, unknown) fails the push with exit code 183.
- Both hooks refuse to run when trufflehog is missing. trufflehog is not in the official Arch repositories; it is installed as `trufflehog-bin` (AUR) on the owner's machines.
- The hooks are small shell scripts because git runs them directly. This is the one exception to [0008](0008-jbang-for-project-tooling.md): there is no tool logic in them, only the trufflehog call.
- `git push` stays a plain command ([0004](0004-plain-commands-and-wrapper-scripts.md)); the hook runs inside git, so the rule is not affected. If verification needs a host that the sandbox does not allow, the scan reports `unknown` and the push fails: run the push outside the sandbox or add the host to the allowed domains.

## Consequences
- A secret is caught before it enters history (commit) and again before it leaves the machine (push).
- Hooks are not cloned automatically and `--no-verify` skips them; a periodic `trufflehog git file://$PWD --results=verified,unknown,unverified` over the whole history is the safety net.
- Every commit and push takes a moment longer; false positives in tracked files must be fixed or excluded with a trufflehog config.
