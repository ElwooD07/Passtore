# Copilot Instructions for Passtore

## Git Commands Policy

**Never run `git commit` or `git push`.** This is a hard rule with no exceptions.

Show the user what changed and which files are staged, but stop there.
The user decides when and what to commit.

Only recommend `git commit` and `git push` commands for the user to run manually.

## Script Execution Policy

The entire project directory is trusted.

Allow running any script that is located inside this project folder when it is needed for the task.

## Shell Preference Policy

Use the following priority order when choosing a shell:

1. **Bash (Git Bash)** — the default for all shell commands.
2. **cmd / .bat scripts** — fall back only when the task cannot be expressed in bash (e.g. invoking a `.bat` script that has no `.sh` equivalent, or when a bash terminal is unavailable).
3. **PowerShell** — use only as a last resort when neither bash nor cmd can accomplish the task.

Reuse an already-open terminal of the preferred type rather than opening a new one.

Do not use `git` commands inside shell terminals. Run `git` commands as standalone tool invocations outside any shell.

## Environment Default

Use Windows paths when invoking native Windows tools. The preferred shell outside Windows is bash. Use POSIX-style paths inside bash sessions (`/mnt/e/...` in WSL, `/e/...` in Git Bash).

## Build Script Policy

The project ships paired scripts: `scripts/build.sh` (bash) and `scripts/build.bat` (cmd). Always prefer the `.sh` variant. Fall back to the `.bat` variant only when bash is genuinely unavailable.

Use the scripts instead of invoking build command lines directly. The scripts set up the environment and ensure a consistent build process across different machines and environments.

## SQLite Policy

Always enable WAL (Write-Ahead Logging) mode for any new SQLite database connection.

Call `PRAGMA journal_mode=WAL;` immediately after opening a real database.

When implementing `BEGIN IMMEDIATE` transactions for write-heavy operations, note that WAL mode requires it to avoid `SQLITE_BUSY` on concurrent readers.

## Error Handling Policy

`void` functions that perform an operation with an expected outcome must throw a `std::runtime_error` on failure — not silently return, not only log.

Logging before throwing is acceptable for diagnostics, but a silent return is never correct when the caller has no way to detect the failure.

## Query::Step() Usage Policy

`Step()` throws on real SQLite errors. Its `bool` return value means "a row is available" (`SQLITE_ROW`) — nothing more.

- Do NOT check `!Step()` to detect "not found" in write operations (Swap, Delete, etc.). Write paths trust that callers pass valid IDs.
- `!Step()` checks are only appropriate in **read operations** that return a status (e.g. `GetOne`, `GetNext`) where "no row" is a valid, expected outcome to propagate up.
- Never throw based on `!Step()` inside a write helper — `Step()` already throws on real failures.

## Precompiled Header Policy

All standard library (`<std>`) includes belong in the target's `pch.h`, not in individual `.cpp` files.
When adding a new std header, add it to the appropriate `pch.h` and do **not** `#include` it in the `.cpp` file.

## No Migration Policy

Until version 1.0 there is no migration of any kind.

Do **not** write migration code for local databases, server databases, config files, or any other storage technology.
When a schema or format changes, the old data is simply incompatible — delete and recreate.
No `ALTER TABLE`, no `PRAGMA table_info` checks, no file-format version fields, no compatibility shims.
