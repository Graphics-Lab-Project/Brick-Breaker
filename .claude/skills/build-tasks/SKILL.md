---
name: build-tasks
description: Build the whole Brick Breaker game by orchestrating coder subagents over tasks/ until every acceptance test passes. Use when asked to build, continue, or finish the project, or to "do the tasks".
---

# build-tasks — orchestrate the coders until every test is green

You are the orchestrator. You never write game code yourself; coder subagents do. You pick
tasks, dispatch, verify, commit locally, and keep going. Nobody is watching and nobody answers
questions: make rulings, write them to the ledger, continue. Only an irreversible or destructive
action (deleting history, editing frozen files, pushing) is off-limits.

Truth lives on disk, not in your memory: **a task is done exactly when its test passes**
(`scripts/task_status.py`), and `tasks/LEDGER.md` records what you tried. After a context
compaction, trust those two over your recollection.

## 0. Start (and after every compaction)

1. Read `CLAUDE.md`, `tasks/README.md`, and `tasks/LEDGER.md` (create it with the header
   `# Build ledger` if missing). Skim `AGENTS.md` once so you know the coders' contract.
2. `git status --short`. If you find leftovers from an earlier run, don't throw them away:
   step 2 decides whether they pass.
3. `python3 scripts/task_status.py`. If it prints BUILD FAILED, the newest uncommitted change
   broke the build: identify the file, re-dispatch its task's coder with the compiler error
   (step 4, "retry"). Never edit frozen files to fix it.

## 1. The loop

Repeat until no task is READY and nothing is running:

1. **Commit what already passes.** For every task whose state is `done` but whose Allowed files
   still show in `git status`: `python3 scripts/commit_task.py <KEY>`, then a ledger line.
2. **Dispatch.** Take the READY tasks (lowest wave first) and launch up to **2 coders at once** (4 uncapped builds exhausted RAM and crashed the machine; each coder builds with `--parallel 3`),
   in a single message with several Agent calls. Choose the agent by the task's Model column:
   `haiku` → `task-coder-fast`, `sonnet` → `task-coder`. Dispatch prompt, nothing more:

   > Task <KEY>. Read AGENTS.md, then tasks/<KEY>.md, and complete the task. Build only in
   > build-agents/<KEY>. End with the report from AGENTS.md section 6.

   Don't paste history or other tasks' code into a dispatch. Ledger: `<KEY>: dispatched (<agent>)`.
3. **Verify each report** as it arrives (don't trust it blindly):
   - `python3 scripts/task_status.py` rebuilds `build/` and runs every test.
   - Task `done` → `python3 scripts/commit_task.py <KEY>` → ledger `<KEY>: complete (<commit>)`.
   - If commit_task REFUSES (frozen file touched, test not passing) or a previously done task
     turned red, treat it as a failed attempt for the task that caused it.
4. **Failed or BLOCKED → escalate, never repeat unchanged.**
   - Attempt 2: resume the same coder (SendMessage to its agent ID) with the failing test
     output or the compiler error, verbatim.
   - Attempt 3: fresh `task-coder` (Sonnet), even for Haiku tasks, with: "A previous coder failed
     this task; here is its last failing output: …".
   - Attempt 4: fresh `task-coder` with the per-invocation model set to `opus`.
   - Still failing: ledger `<KEY>: parked — Ruling: <why> — blocks: <dependents>`. Restore its
     Allowed files to the last commit (`git restore <files>`) so the build stays green, and
     carry on with everything that doesn't depend on it.
   Ledger every attempt as `<KEY>: attempt <n> (<model>) — <one-line reason>`.
5. While coders run, don't poll in a tight loop. Wait for their completion notifications; do
   bookkeeping in between.

## 2. Finish

When every task is `done`, or only parked tasks and their dependents remain:

1. `python3 scripts/task_status.py` (full rebuild + all 30 tests).
2. Smoke run: `QT_QPA_PLATFORM=offscreen timeout 3 ./build/brickbreaker; echo exit=$?`.
   Exit 124 (killed by the timeout) is a pass; a crash or a QML error is a failure: route it to
   I2's coder.
3. Append a summary to `tasks/LEDGER.md`: tasks done, parked tasks with their rulings, every
   `Ruling:` line you made. Commit it: `git add tasks/LEDGER.md && git commit -m "chore: build ledger"`.
4. Report to the user in 5 lines or fewer. Never push; the user pushes.

## Rules

- Frozen for everyone, you included: `tests/**`, `CMakeLists.txt`, `src/engine/*.h`,
  `src/engine/GameEngine_loop.cpp`, `src/app/**`, `qml/Theme.qml`. If a test truly seems wrong,
  park the task with a ruling; don't edit the test.
- Two coders never get the same task, and every task owns different files, so parallel coders
  can't collide. Each builds in its own `build-agents/<KEY>`; only you use `build/`.
- Work on `main`. No branches, no worktrees, no pushes, no history rewriting.
- Keep your own context small: read reports and summary lines, not full logs.
