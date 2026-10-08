# Brick Breaker (Qt 6 QML + C++)

A BlackBerry-style Brick Breaker: 20 levels, power-up capsules (extra life, multi-ball, long paddle, laser, gun), a level
selector with unlockable levels, and saved high scores with play times. Pixel look, no images and no sound.
Built for the Computer Graphics Lab, written by Claude Code coder agents.

## 1. Install

You need a C++17 compiler, CMake 3.21+, Ninja, and **Qt 6.4+** with Quick/QML, Test and **SQL (SQLite driver)**.

| System | Command |
|---|---|
| Fedora | `sudo dnf install gcc-c++ cmake ninja-build qt6-qtbase-devel qt6-qtdeclarative-devel` |
| Ubuntu / Debian | `sudo apt install g++ cmake ninja-build qt6-base-dev qt6-declarative-dev libqt6sql6-sqlite qml6-module-qtquick qml6-module-qttest` |
| macOS (Homebrew) | `brew install cmake ninja qt` |
| Windows | Qt 6 from the Qt installer + CMake + Ninja |

## 2. Build

```bash
cmake -S . -B build -G Ninja         # add -DCMAKE_PREFIX_PATH=<path-to-Qt6> if Qt is not found
cmake --build build --parallel 4     # keep the job count modest, each compile job is memory hungry
```

## 3. Play

```bash
./build/brickbreaker                 # or ./run-game.sh (builds if needed, then starts the game)
```

`run-game.sh` can also back a desktop launcher: point a `.desktop` file's `Exec=` at it.

| Action | Keys / mouse |
|---|---|
| Move paddle | `Left` / `Right`, or move the mouse |
| Launch ball / fire (gun, laser) | `Space`, or click |
| Pause | `P` or `Esc` (pause menu: Options, Quit to menu) |
| Menus | `Up` / `Down`, `Enter` or `Space` to pick, `Esc` to go back, mouse works too |
| Fullscreen / windowed | `F11` (starts fullscreen) |

START GAME (CONTINUE after your first game) opens the level select. A level unlocks when you clear the one before it.
Your best score and best time per level show in that list.

**Dev switch:** `qml/Main.qml` sets `unlockAll: true`, so every level is open. Delete that line to play with real locking.

**Saved data:** options, unlocked levels and the top-5 initials table are saved with `QSettings`
(`~/.config/GraphicsLab/BrickBreaker.conf` on Linux). Level clears and finished runs, with play time in microseconds,
go into SQLite: `~/.local/share/GraphicsLab/BrickBreaker/scores.sqlite` on Linux. Delete both to reset.

## 4. Test

```bash
ctest --test-dir build --output-on-failure     # 36 engine (QtTest) and UI (Qt Quick Test) tests, headless
python3 scripts/task_status.py                 # rebuild, run every test, show which tasks are done
```

## 5. Repo map

| Path | What |
|---|---|
| `src/engine/` | Game logic in C++, deterministic and fixed-timestep: physics, bricks, 20 levels, power-ups, state machine, `GameEngine`, score persistence (`ScoreStore`, `SqliteScoreStore`) |
| `src/app/main.cpp` | Application entry point |
| `qml/` | All screens and components (QML only renders and forwards input); `Theme.qml` holds colours and sizes |
| `fonts/` | Silkscreen pixel font (licence: `OFL.txt`) |
| `tests/engine/`, `tests/qml/` | One acceptance test per task; `tests/qml/MockEngine.qml` stands in for the engine in UI tests |
| `docs/` | `PLAN.md` architecture and numbers, `DESIGN_HANDOFF.md` look and motion, `LEVEL_SELECT.md`, `PERSISTENCE.md` feature plans |
| `tasks/` | One spec per task (`tasks/README.md` is the index), plus `LEDGER.md`, the build log |
| `scripts/` | `task_status.py`, `commit_task.py` for the agent workflow |
| `AGENTS.md`, `CLAUDE.md`, `.claude/` | Rules and tooling for the Claude Code agent workflow (`/build-tasks`) |
| `run-game.sh` | Build-and-run launcher |

## 6. Build it with agents

Open Claude Code in this folder and run `/build-tasks`: an orchestrator dispatches Sonnet/Haiku coder subagents, one task each,
until every acceptance test passes. Coders follow `AGENTS.md`; the orchestrator follows `CLAUDE.md`.

License: MIT (`LICENSE`); the bundled font is under the SIL Open Font License (`OFL.txt`).
