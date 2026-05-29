# Copilot Instructions for Passtore

## Git Commands Policy

**Never run `git commit` or `git push`.** This is a hard rule with no exceptions.

Show the user what changed and which files are staged, but stop there.
The user decides when and what to commit.

Only recommend `git commit` and `git push` commands for the user to run manually.

## Code Scope Policy

Only analyse and modify files under the `src/` and `.github/` folders.
Do **not** read, edit, or reference anything under `build/`, `ext/`, `plan/`, or `scripts/` unless the task is explicitly about those directories.

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

## C++ Code Style

### Naming
- **Classes, structs, type aliases, file names**: PascalCase — `ResourceTableModel`, `TableSettings`, `ResourceId`
- **Methods**: PascalCase — `RowCount()`, `SetCellData()`, `IsVisibleColumn()`
- **Member variables**: `m_` prefix + camelCase — `m_model`, `m_resourcesDefs`, `m_btnDelete`
- **File-scope and in-class constants**: `s_` prefix + camelCase — `s_kdfIterations`, `s_insertStr`. UPPER_SNAKE_CASE is reserved for preprocessor `#define` macros only.
- **Namespaces**: lowercase — `passtore`, `sqlite`
- **Free functions in anonymous namespaces**: PascalCase — `TestsDir()`, `MakeResource()`. Any private logic that does not touch class members must be a free function in an anonymous namespace rather than a private method.

### Braces (Allman style)
- Opening brace on its own line for functions, classes, structs, namespaces, and control flow.
- Exception: empty bodies inline — `virtual ~IResourceStorage() { }`, `{ }` on one line.
- **Always** use braces for `if`/`else`/`for`/`while`/`do-while`, even for single-statement bodies.

### Indentation
- 4 spaces; no tabs.

### Headers
- `#pragma once` — no include guards.
- Include order: PCH (`"pch.h"`) first in `.cpp` files, then standard library headers, then third-party headers, then project headers. Blank line between groups.
- Each `.cpp` starts with `#include "pch.h"` (or `#include <pch.h>` for library targets).

### Namespaces
- Opening brace on same line as `namespace` keyword (`namespace passtore\n{`).
- `using namespace passtore;` at file scope in `.cpp` files — never in headers.

### Classes
- Access order: `public` → `protected` → `private`.
- Two separate `private:` sections are acceptable: one for methods, one for data members.
- Use `struct` for plain data aggregates; use `class` for types with behaviour and invariants.
- Mark single-argument constructors `explicit`.
- Delete copy constructor/assignment for non-copyable types with `= delete`.
- Default move constructor/assignment with `= default` where appropriate.
- Mark all overrides with `override`; omit redundant `virtual` on overriding declarations.

### Constructor initializer lists
- Each initializer on its own line, preceded by `:` or `,` aligned under the constructor name:
  ```cpp
  MyClass::MyClass(int x, int y)
      : m_x(x)
      , m_y(y)
  {
  ```

### Pointers and references
- `*` and `&` adjacent to the type, not the name — `int* p`, `const std::string& s`.

### `const` placement
- West const for parameters and return types — `const std::string& s`, `const auto& def`.
- `const` member functions marked with trailing `const`.

### `auto`
- Use `auto` for iterators, range-based for, and when the type is immediately obvious from the right-hand side.
- Spell out the type for member variable declarations and function parameters.

### Casts
- Use named casts only: `static_cast`, `reinterpret_cast`. Never C-style casts.

### Initialization
- Prefer brace initialization `{}` for structs and aggregate construction — `ColumnSettings col{ name, true }`.
- Use `= nullptr` for pointer member defaults.
- Use `= 0` / `= false` / `= true` for scalar member defaults.
- Use `constexpr` for compile-time constants with no runtime dependency; `static const` otherwise.

### `nullptr`
- Always `nullptr`; never `NULL` or `0` for pointers.

### Comments
- Each `class` and `struct` must have a short `/* */` block comment immediately above its declaration describing why it exists, what it does, and (if non-obvious) how. Keep it to one or two sentences.
- Inline `//` comments should be the exception, not the rule. Prefer self-documenting names; add a `//` comment only when the name alone cannot convey the intent.
- No Doxygen annotations.
- `// TODO:` prefix for known deferred work.

### Error handling
- Throw `std::runtime_error` on unrecoverable failures (see Error Handling Policy above).
- Use early returns to reduce nesting; guard clauses at the top of a function.
- Catch by `const std::exception&`.

### Tests (Google Test)
- Helper factories and utilities in an anonymous `namespace { }` at the top of the test file.
- Test DB files go in `TestsDir() / "test_*.db"`.
- One logical concept per `TEST()` macro; descriptive names in `Subject_Expectation` form.

### Warnings as errors
Every compiler warning is treated as a build error (`/WX` on MSVC). Style choices that produce warnings — signed/unsigned comparison, unused variables, implicit conversions, etc. — will break the build and must be fixed.
