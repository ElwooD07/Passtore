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

1. **Git Bash** — preferred for all shell commands when available.
2. **cmd** — use when Git Bash is not available.
3. **PowerShell** — use only when neither Git Bash nor cmd can accomplish the task.
4. **WSL** — last resort only. See WSL Usage Policy below.

Do not use `git` commands inside Bash/cmd/PowerShell terminals. Run `git` commands as standalone tool invocations outside any shell.

## Environment Default

Assume developers are working locally on Windows by default.

Use native Windows paths/commands first and do not suggest WSL-specific steps unless the user explicitly asks for WSL.

## WSL Usage Policy

WSL is the last resort. Only use it for Unix utilities that have no native Windows equivalent and cannot run in Git Bash.

For build tools, compilers, and project scripts that have native Windows versions (MSBuild, cmake, .bat scripts), always invoke them natively — not through WSL.

Prefer `scripts\build.bat` over calling MSBuild via `/mnt/c/...` paths in WSL.

Never mix WSL paths (e.g. `/mnt/e/...`) with native Windows tool invocations unless the tool explicitly requires it.

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
