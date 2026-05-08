# Copilot Instructions for Passtore

## Git — DO NOT COMMIT

**Never create git commits.** This is a hard rule.

Show the user what changed and which files are staged, but stop there.
The user decides when and what to commit.

The only exception is if the user explicitly says "commit this" or "commit now" for a specific change.

## Environment Default

Assume developers are working locally on Windows by default.

Use native Windows paths/commands first and do not suggest WSL-specific steps unless the user explicitly asks for WSL.
