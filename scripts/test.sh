#!/usr/bin/env bash
# Configure, build, and run all tests.
# Usage: scripts/test.sh [Debug|Release]
#   Default config is Debug.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build/cmake"
CONFIG="${1:-Debug}"

resolve_tool()
{
    local tool="$1"
    local fallback="$2"

    if command -v "$tool" >/dev/null 2>&1; then
        command -v "$tool"
        return 0
    fi

    if command -v "$fallback" >/dev/null 2>&1; then
        command -v "$fallback"
        return 0
    fi

    return 1
}

CMAKE_CMD="$(resolve_tool cmake cmake.exe || true)"
CTEST_CMD="$(resolve_tool ctest ctest.exe || true)"

SOURCE_DIR_ARG="$PROJECT_ROOT"
BUILD_DIR_ARG="$BUILD_DIR"

# If we are in WSL but only Windows CMake is available, convert paths.
if [[ -n "${WSL_DISTRO_NAME:-}" && "$CMAKE_CMD" == *.exe ]]; then
    SOURCE_DIR_ARG="$(wslpath -w "$PROJECT_ROOT")"
    BUILD_DIR_ARG="$(wslpath -w "$BUILD_DIR")"
fi

if [[ -z "$CMAKE_CMD" || -z "$CTEST_CMD" ]]; then
    echo "Error: cmake/ctest not found in PATH."
    echo "Install CMake in WSL or add Windows CMake to PATH so cmake.exe/ctest.exe are visible."
    exit 1
fi

echo "==> Configuring ($CONFIG)..."
"$CMAKE_CMD" \
    -B "$BUILD_DIR_ARG" \
    -S "$SOURCE_DIR_ARG" \
    -DCMAKE_PREFIX_PATH="C:/Qt/6.1.1/msvc2019_64"

echo "==> Building tests ($CONFIG)..."
"$CMAKE_CMD" --build "$BUILD_DIR_ARG" --config "$CONFIG"

echo "==> Running tests ($CONFIG)..."
"$CTEST_CMD" \
    --test-dir "$BUILD_DIR_ARG" \
    -C "$CONFIG" \
    --output-on-failure

echo "==> All tests finished."
