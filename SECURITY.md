# Security Policy

## Supported Versions

JARVIS Lite is pre-release software until the first tagged public release. Security fixes will target the latest main branch unless release branches are introduced.

## Reporting a Vulnerability

Do not open public issues for sensitive vulnerabilities. Report privately to the project maintainers once a public security contact is configured.

Include:

- Affected version or commit
- Operating system
- Reproduction steps
- Expected impact
- Any relevant logs with secrets removed

## Security Principles

- Offline-first behavior by default.
- No bundled model files.
- No unnecessary network calls.
- Confirm terminal command execution.
- Do not log secrets or full sensitive file contents.
- Keep platform-specific capabilities isolated and reviewable.

