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

Prefer Bash for all commands whenever possible.

Do not use PowerShell if the same command can be performed with Bash.

Do not use Bash for `git` commands.
Run `git` commands outside Bash.

## Environment Default

Assume developers are working locally on Windows by default.

Use native Windows paths/commands first and do not suggest WSL-specific steps unless the user explicitly asks for WSL.
