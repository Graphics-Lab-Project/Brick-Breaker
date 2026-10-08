#!/usr/bin/env bash
# Build (incremental) and launch Brick Breaker.
set -e
cd "$(dirname "$(readlink -f "$0")")"
LOG="${XDG_CACHE_HOME:-$HOME/.cache}/brickbreaker-build.log"
mkdir -p "$(dirname "$LOG")"
{
    [ -f build/CMakeCache.txt ] || cmake -S . -B build -G Ninja
    cmake --build build --parallel 4
} >"$LOG" 2>&1 || {
    command -v notify-send >/dev/null && notify-send "Brick Breaker" "Build failed, see $LOG"
    echo "Build failed, see $LOG" >&2
    exit 1
}
exec ./build/brickbreaker "$@"
