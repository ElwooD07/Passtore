#!/usr/bin/env bash
# Run the Passtore executable.
# Usage: scripts/run.sh [Debug|Release]
#   Default config is Debug.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
CONFIG="${1:-Debug}"
EXE="$PROJECT_ROOT/build/cmake/bin/$CONFIG/Passtore.exe"

if [[ ! -f "$EXE" ]]; then
    echo "Error: executable not found at $EXE"
    echo "Run scripts/build.sh first."
    exit 1
fi

# Qt DLLs are deployed next to the exe by build.sh (windeployqt)
echo "==> Running $EXE"
"$EXE"
