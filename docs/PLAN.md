# PLAN — Brick Breaker (Qt 6 QML + C++)

Phase 0 output. Read with `AGENTS.md` (the contract) and `docs/DESIGN_HANDOFF.md` (look and motion).
Task specs live in `tasks/<KEY>.md`; the index is `tasks/README.md`.

## Architecture (one page)

```
Main.qml ── FrameAnimation ──> GameEngine.tick(frameTime)        (QML drives time, nothing else)
   └─ AppRoot ─ MenuScreen / OptionsScreen / HelpScreen / GameScreen
                                                      ├─ Hud, PauseButton, InputHint
                                                      ├─ Playfield ─ Board ─ Brick×98, Capsule, Projectile, Ball, Paddle
                                                      ├─ FxLayer (T1, reacts to engine signals)
                                                      └─ PauseOverlay, LevelBanner, GameOverOverlay(+InitialsEntry)

GameEngine (QObject, QML_ELEMENT)  — properties + signals + invokables = docs/DESIGN_HANDOFF.md §4
   tick(dt): clamp dt to 0.05 s, run fixed steps of 1/120 s:
      stepInput → stepStateTimers → [Playing] stepBalls → stepPowerUps → syncModels
   owns pure, separately tested logic:
      PaddleLogic · BallPhysics · Collision · BrickGrid + Levels · CapsuleSystem(RandFn)
      PowerUps · WeaponSystem · PaceController · GameStateMachine · HighScoreTable
   exposes 4 QAbstractListModels: bricks · balls · capsules · projectiles
```

- **All gameplay is C++ and deterministic** (fixed timestep, injected RNG). QML only renders, animates (T1 behind `polish`) and forwards input.
- **Every file has exactly one owner task**, so parallel coder subagents never collide. `GameEngine` is split over four `.cpp` files for that reason; `GameEngine_loop.cpp` holds the frozen shared helpers.
- **Tests come first.** Every task has one test file committed in Phase 0 (red now). Agents make them green and never edit them.
- **UI tests use `tests/qml/MockEngine.qml`**, so UI work does not wait for the engine. Only `tst_app_root` uses the real engine.
- Coordinates: logical 360×480 canvas (window 720×960, `scale: 2`); game coordinates are relative to the 336×336 playfield at (12, 104).

## How the build runs

Solo and local. The main Claude Code session runs the `build-tasks` skill
(`.claude/skills/build-tasks/SKILL.md`) and acts as orchestrator: it dispatches coder subagents
(`.claude/agents/task-coder*.md`, Sonnet or Haiku per task), verifies with
`scripts/task_status.py` (a task is done exactly when its test passes), commits each finished
task locally with `scripts/commit_task.py`, and loops until all tests pass. No branches, PRs or
human review. CI (`.github/workflows/ci.yml`) builds and runs all 30 tests once the code is pushed.

## Gameplay numbers (approved; all in `src/engine/Constants.h`)

| Topic | Value |
|---|---|
| Timestep | fixed 1/120 s; `tick(dt)` clamps dt to [0, 0.05] |
| Ball | Slow 180 px/s; Fast 260 px/s after 50 paddle hits in a life; reset to Slow on life lost / level start |
| Paddle bounce | angle from vertical = clamp(offset/half-width) × 60° |
| Launch | 30° off vertical toward paddle motion (right when still) |
| Paddle speed | settings 1–5 = 120/180/240/300/360 px/s; acceleration ramp 150 ms (keyboard); mouse = absolute X |
| Capsules | 1 in 6 broken bricks, 5 types uniform, fall 80 px/s, +50 on catch, 500 ms catch flash for every type |
| Scoring | brick hit 10 · gun kill 50 · laser hit 10 · silver hit by ball 0 |
| Gun | 3 shots, destroys any brick incl. silver, 400 px/s |
| Laser | twin bolts, 1 hit each, no effect on silver, 0.25 s cooldown, lasts until life lost / new mode |
| Modes | Long/Gun/Laser replace each other; Life = +1 life (max 9) and ends other powers |
| Multi | 4 balls at −50/−20/20/50° from vertical, from ball 0's position |
| Wall descent | while Fast, each paddle hit drops the wall 1 row (24 px), max 4 |
| Lives / levels | 3 lives; silver does not block clearing; 1.5 s banner; 10 levels then loop with round+1 |
| High scores | top 5, 3-letter initials, QSettings |

v1 scope decisions: everything drawn with QML `Rectangle`/`Text` (no PNG assets), no audio, Silkscreen font bundled by I2, levels 2–10 are the approved placeholders.

## Tasks and waves

The full table (dependencies, model, owned files, test) is `tasks/README.md`.

- **Wave 1** (22 tasks, no dependencies): engine modules E1–E11 and UI components U1–U11.
- **Wave 2:** G1 engine core, G2 ball step, G3 power-ups (need their engine modules), U12 playfield (needs U1–U3).
- **Wave 3:** I1 game screen (needs the UI components and U12).
- **Wave 4:** I2 app root + main window (needs everything).

## Phase 0 verification

The tests were checked against a private reference implementation (not committed). With it, all 30 ctest tests passed on Qt 6.4 (Ubuntu) in 3 consecutive parallel runs; with the committed stubs, only the 2 sanity tests pass. CI uses Qt 6.8.
