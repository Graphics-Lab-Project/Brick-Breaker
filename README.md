# Brick Breaker (Qt 6 QML + C++)

BlackBerry-style Brick Breaker clone for the Computer Graphics Lab, built by four people, each running a coding agent on one GitHub issue at a time.

- **Start here:** `AGENTS.md` (the contract every agent and human follows).
- **Plan, architecture, gameplay numbers, task list:** `docs/PLAN.md`.
- **Look and motion:** `docs/DESIGN_HANDOFF.md`.
- **Issues / board / assignments:** `tools/create_issues.py` (run by the integrator; `--help`).
- **Workflow:** `docs/EXAMPLE_FLOW.md`, setup in `AGENT_SETUP.md` and `docs/GITHUB_SETUP_GUIDE.md`.

```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="<path-to-Qt6>"
cmake --build build
ctest --test-dir build                     # most tests are red until their tasks land
./build/brickbreaker                       # the game (a black window until integration)
```
