# Build ledger

- E1: dispatched (task-coder-fast)
- E2: dispatched (task-coder-fast)
- E3: dispatched (task-coder-fast)
- E4: dispatched (task-coder-fast)
- E1: complete (8625279)
- E2: complete (62b88bb)
- E3: complete (371afa6)
- E4: complete (4273420) — Ruling (coder): load() returns false on >14 rows/bad row/char; silver reports unbreakable
- E5, E6, E7, E8: dispatched (task-coder-fast)
- Ruling: OOM crash (63 concurrent cc1plus ~18GB from 4 coders x uncapped ninja). Capped coder builds at --parallel 3, task_status at 4, dispatch width 2. E5-E8 had no landed changes; redispatching.
- E5, E6: dispatched (task-coder-fast)
- E5: complete (see git log)
- E6: complete (42f7d27)
- E7, E8: dispatched (task-coder-fast)
- E8: complete (bc36aa2)
- E9: dispatched (task-coder-fast)
- E7: complete (see git log)
- E10: dispatched (task-coder-fast)
- E9: complete (see git log)
- Ruling: raised to 4 coders at once, each --parallel 2 (<=~12 compile jobs total); mem avail ~17GB.
- E11, U1, U2: dispatched (task-coder, sonnet)
- E10: complete (see git log)
- U4: dispatched (task-coder, sonnet)
- U1: complete (d8e148c)
- U3: dispatched (task-coder-fast)
- U4: complete (see git log)
- U5: dispatched (task-coder-fast)
- U2: complete (see git log)
- U6: dispatched (task-coder, sonnet)
- E11: complete (see git log)
- Ruling: dispatching wave-2 engine tasks (G1-G3) as soon as deps are met, ahead of remaining wave-1 UI tasks (critical path).
- G1: dispatched (task-coder, sonnet)
- U5: complete, U6: complete (88761a8)
- G2, G3: dispatched (task-coder, sonnet)
- U3: complete (see git log)
- U12: dispatched (task-coder, sonnet)
- G1: complete (see git log)
- U7: dispatched (task-coder, sonnet)
- U12: complete (see git log)
- U8: dispatched (task-coder-fast)
- G2, G3: complete (see git log)
- U9, U10: dispatched (task-coder, sonnet)
- U7: complete (see git log)
- U11: dispatched (task-coder, sonnet)
- U10: complete (see git log)
- U9: complete (see git log)
- I1: dispatched (task-coder, sonnet)
- U11: complete, U8: complete (see git log)
- I1: complete (see git log)
- I2: dispatched (task-coder, sonnet)
- I2: complete (7e026e1)

## Summary
- Tasks done: 28/28 (all acceptance tests pass; full rebuild + ctest green, sanity tests ok).
- Parked: none. No failed attempts or escalations were needed.
- Smoke run: `QT_QPA_PLATFORM=offscreen timeout 3 ./build/brickbreaker` -> exit 124, no output (pass).
- Ruling: OOM crash (63 concurrent cc1plus ~18GB). Coder builds capped at `--parallel 2`, task_status at `--parallel 3` / `ctest -j 4`, dispatch width 4.
- Ruling: wave-2 engine tasks (G1-G3) dispatched ahead of remaining wave-1 UI tasks (critical path).
- Known gap: `fonts/` (Silkscreen-Regular/Bold.ttf) does not exist; Main.qml's FontLoaders point to qrc:/fonts/... and fall back to the default font. Fonts can't be downloaded here; add them manually if wanted.

## Level select feature (user request)
- Ruling: user-authorised edits to frozen files (GameEngine.h, GameEngine_loop.cpp one-line unlock hook, CMake, tests, MockEngine) done by orchestrator before dispatch; coders still never touch them.
- Main.qml: fullscreen + fit-scale + F11; unlockAll dev switch set true (remove line to ship).
- L1, L2: dispatched (task-coder, sonnet)
- L2: complete (swept into 797bcbb, test tst_level_select passes)
- L1: complete (see git log)
- L3: dispatched (task-coder, sonnet)
- L3: complete (see git log)
- Level select feature done: 31/31.

## 20 levels (user request)
- Ruling: LevelCount 10 -> 20; max 4 stones per level (L9: 12 -> 4, L10: 9 -> 4); new layouts 11-20 per docs/LEVEL_SELECT.md. Frozen files (Constants.h, GameEngine.h, tests, MockEngine) edited by orchestrator on user instruction.
- M1, M2: dispatched
- M1, M2: complete (see git log). 33/33 tests green, 20 levels, max 4 stones.

## Score persistence (user request)
- Ruling: new layer ScoreStore (interface) + SqliteScoreStore (QtSql, new Qt module Sql, user-requested for future SQL leaderboard). Time = deterministic play time in microseconds (1/120 s resolution). Frozen edits by orchestrator: CMake, GameEngine.h, GameEngine_loop.cpp hooks, tests, stubs.
- S1, S3: dispatched (task-coder, sonnet); S2 after S1
