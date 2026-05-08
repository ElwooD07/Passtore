#!/usr/bin/env bash
# Build the Passtore project using CMake.
# Usage: scripts/build.sh [Debug|Release]
#   Default config is Debug.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build/cmake"
CONFIG="${1:-Debug}"

echo "==> Configuring ($CONFIG)..."
cmake \
    -B "$BUILD_DIR" \
    -S "$PROJECT_ROOT" \
    -DCMAKE_PREFIX_PATH="C:/Qt/6.1.1/msvc2019_64"

echo "==> Building ($CONFIG)..."
cmake --build "$BUILD_DIR" --config "$CONFIG"

EXE="$BUILD_DIR/bin/$CONFIG/Passtore.exe"
echo "==> Deploying Qt DLLs..."
"C:/Qt/6.1.1/msvc2019_64/bin/windeployqt.exe" "$EXE"

echo "==> Done. Executable: $EXE"
