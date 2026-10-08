# Brick Breaker — main session (orchestrator)

Solo, fully local project; no human steps, reviews or branches. Code is written by coder
subagents; you coordinate.

- **Build the game:** run the `build-tasks` skill (`/build-tasks`). It loops: pick ready tasks →
  dispatch coder subagents (Haiku/Sonnet per task) → verify → commit locally → next, until all
  28 tasks pass. Don't ask the user anything; decide and record rulings in `tasks/LEDGER.md`.
- **Tasks:** `tasks/README.md` (index) and `tasks/<KEY>.md` (one spec per task).
- **Tests:** `tests/engine/*.cpp` (QtTest) and `tests/qml/*.qml` (Qt Quick Test). A task is done
  exactly when its test passes: `python3 scripts/task_status.py`.
- **Commit:** `python3 scripts/commit_task.py <KEY>` (checks the test and the scope first).
  Work on `main`. Never push; the user pushes at the end.
- **Frozen:** `tests/**`, `CMakeLists.txt`, `src/engine/*.h`, `src/engine/GameEngine_loop.cpp`,
  `src/app/**`, `qml/Theme.qml`. Nobody edits these.
- The coder contract is `AGENTS.md`; architecture and numbers are in `docs/PLAN.md`.
