# AGENTS.md — coder contract (read fully before every task)

Project: BlackBerry-style Brick Breaker, Qt 6 QML + C++. Solo, local project: no human is
watching or answering. You are a **coder subagent**: you get ONE task key (e.g. `E4`), you make
its acceptance test pass inside its allowed files, you report. The orchestrator (main session)
checks your work, commits it and dispatches the next task. Other coders may be editing other
files in this same checkout at the same time.

## 1. Where everything is

| What | Where |
|---|---|
| **Your task** (goal, allowed files, interfaces, test, spec notes) | `tasks/<KEY>.md` |
| All tasks, dependencies, models | `tasks/README.md` |
| **Your acceptance test** (read-only) | engine: `tests/engine/<test>.cpp` · UI: `tests/qml/<test>.qml` |
| Fake engine that UI tests pass in as `engine` | `tests/qml/MockEngine.qml` |
| Frozen C++ interfaces (header comments = spec) | `src/engine/*.h` |
| Every tuning number | `src/engine/Constants.h` |
| Shared engine helpers you **call**, never re-implement | `src/engine/GameEngine_loop.cpp` |
| Colours, sizes, enum ints, `capsuleColor()`, `capsuleName()`, `pad()` | `qml/Theme.qml` |
| Architecture + gameplay numbers | `docs/PLAN.md` |
| Look, motion timings, level layouts | `docs/DESIGN_HANDOFF.md` |

## 2. Your loop

1. Read `tasks/<KEY>.md`, then every header/stub it names, then the acceptance test itself.
   The test is the definition of done; the Spec notes explain it.
2. Build in **your own** build directory (never `build/`, the orchestrator owns it):
   ```bash
   cmake -S . -B build-agents/<KEY> -G Ninja >/dev/null     # first time only
   cmake --build build-agents/<KEY> --parallel 2     # always cap jobs: many coders share this machine (an uncapped build caused an OOM crash)
   ctest --test-dir build-agents/<KEY> --output-on-failure -R "^<test>$"
   ```
   Confirm it fails for the expected reason (missing behaviour). Keep the summary line.
3. Implement, only in the task's **Allowed files**. Smallest change that satisfies the test and
   the Spec notes.
4. Rebuild and re-run until the test passes. Then run it once more and keep the summary line.
5. `git status --porcelain`: every path *you* changed must be in your Allowed files. Undo any
   stray edit of yours by editing the file back.
6. Reply with the report in section 6. That reply is your last message.

Keep working until the test passes or you are truly blocked; do not stop to ask whether to
continue. When the test passes and the scope check is clean, stop and report. Don't add features,
tests, files, docs or refactors that weren't asked for; mention ideas in NOTES instead.

Run a real check before reporting done: the acceptance test above, built and executed. A
syntax-only check, or a build/ctest command that failed to start, does not count. Never install
anything (no `sudo`, `dnf`, `apt`, `pip`, downloads). If the build environment itself is broken
(Qt or CMake missing), report BLOCKED with the exact error.

## 3. Hard rules

- **R1 Scope.** Change only the task's Allowed files. Everything else is read-only, including
  other tasks' files, `CMakeLists.txt`, `tests/**`, `src/engine/*.h`,
  `src/engine/GameEngine_loop.cpp`, `src/app/**`, `qml/Theme.qml`, `tasks/**`, docs.
- **R2 Tests are law.** Never edit, delete, skip, `QSKIP`, loosen or work around a test. Never
  special-case test values in code (no `if (x == 168)` tricks); derive behaviour from the spec
  and `Constants.h` / `Theme.qml`.
- **R3 Frozen interfaces.** Don't add/remove/rename anything declared in a header or in a QML
  stub's property/signal/function block, and keep every required `objectName`. You may add
  file-local helpers (`static` functions or an anonymous namespace in your `.cpp`; inline
  `component Foo: Rectangle { … }` in your `.qml`). You cannot add new files.
- **R4 Git.** Read-only git only (`status`, `diff`, `log`). The orchestrator commits. Never
  commit, push, reset, stash, checkout, switch or restore.
- **R5 No subagents, no questions.** Don't spawn agents. Nobody will answer a question: decide
  using the spec, or report BLOCKED.
- **R6 Honest results.** Never claim a pass you did not see in this session.

## 4. When you are stuck

Report `STATUS: BLOCKED` (do not keep thrashing) when:
- the same failure survives **3 genuinely different fix attempts**, or
- the test cannot pass without changing a file outside your Allowed files, or
- an interface or the test contradicts the spec.

Put the failing output, what you tried, and your best hypothesis in the report. The orchestrator
will retry with more context or a stronger model.

## 5. Code standards

- **Logic vs. rendering.** Gameplay (physics, collisions, scoring, state, timing) is C++ and
  deterministic. QML renders and forwards input. QML timers/animations are presentation only.
- **Fixed timestep.** Logic advances only through `step(h)` / `tick(dt)`. Never read the wall
  clock, never use `std::rand`; randomness comes from the injected `RandFn`.
- **Engine tasks** call the Phase 0 helpers (`addScore`, `syncPower`, `syncPace`, `loseLife`,
  `enterLevelCleared`, `checkLevelCleared`, `updateBrick`, `syncModels`, …).
- **QML:** declarative bindings over imperative JS. In Repeater delegates use `model.x` /
  `model.y` (roles shadow `Item.x/y`). Everything is drawn with `Rectangle`/`Text`
  (no images, no audio).
- **T1 polish** effects only when the component's `polish` property is true; with
  `polish: false` the component must be fully correct and static.
- Integer enums: QML compares ints (`Theme.statePlaying`, `Theme.modeGun`, …), values as in
  `src/engine/Types.h`.
- **C++:** Qt 6, C++17, follow the formatting of neighbouring files, no new Qt modules.

## 6. Report (your final message, exactly this shape)

```
STATUS: DONE | BLOCKED
TASK: <KEY>
FILES CHANGED: <path>, <path>
TEST BEFORE: <ctest summary line, e.g. "0% tests passed, 1 tests failed out of 1">
TEST AFTER:  <ctest summary line, e.g. "100% tests passed, 0 tests failed out of 1">
NOTES: <max 3 lines: rulings you made, or the blocker with failing output + hypothesis>
```
