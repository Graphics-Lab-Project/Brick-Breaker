# Brick Breaker (Qt 6 QML + C++)

BlackBerry-style Brick Breaker clone for the Computer Graphics Lab, built locally by Claude Code: an orchestrator session dispatches Sonnet/Haiku coder subagents, one task at a time per coder.

- **Build it with agents:** open Claude Code in this folder and run `/build-tasks`
  (orchestrator skill; see `CLAUDE.md`). Coders follow `AGENTS.md`.
- **Tasks:** `tasks/README.md` and `tasks/<KEY>.md` · **Status:** `python3 scripts/task_status.py`
- **Plan, architecture, gameplay numbers:** `docs/PLAN.md` · **Look and motion:** `docs/DESIGN_HANDOFF.md`

```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="<path-to-Qt6>"
cmake --build build
ctest --test-dir build                     # red until each task is done
./build/brickbreaker                       # the game (a black window until integration)
```
