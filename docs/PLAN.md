# PLAN — Brick Breaker (Qt 6 QML + C++)

Phase 0 output. Read with `AGENTS.md` (the contract) and `docs/DESIGN_HANDOFF.md` (look and motion).
Issue titles start with the task key (e.g. `[E4]`), so keys below map 1:1 to GitHub issues.

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
- **Every file has exactly one owner task**, so parallel PRs never conflict. `GameEngine` is split over four `.cpp` files for that reason; `GameEngine_loop.cpp` holds the frozen shared helpers.
- **Tests come first.** Every task has one test file committed in Phase 0 (red now). Agents make them green and never edit them.
- **UI tests use `tests/qml/MockEngine.qml`**, so UI work does not wait for the engine. Only `tst_app_root` uses the real engine.
- Coordinates: logical 360×480 canvas (window 720×960, `scale: 2`); game coordinates are relative to the 336×336 playfield at (12, 104).

## CI and the green set

`build-test` builds everything and runs the tests named in `ci/green/` (required). The rest of the suite also runs but is informational, because unfinished tasks are red by design. Each task adds its own `ci/green/<test>` marker, so finished work stays protected. `scope-check` fails a PR that touches files outside its issue's **Allowed files**.

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

Wave 1 has no dependencies (22 tasks; split them across the 4 people). Waves 2–4 unlock as their blockers merge.

| Wave | Key | Task | Owns | Test | Blocked by | Person |
|---|---|---|---|---|---|---|
| 1 | E1 | Engine: collision helpers | `src/engine/Collision.cpp` | `tst_collision` | — | B |
| 1 | E2 | Engine: paddle logic | `src/engine/PaddleLogic.cpp` | `tst_paddle_logic` | — | C |
| 1 | E3 | Engine: ball physics | `src/engine/BallPhysics.cpp` | `tst_ball_physics` | — | B |
| 1 | E4 | Engine: brick grid + 10 levels | `src/engine/BrickGrid.cpp`, `src/engine/Levels.cpp` | `tst_brick_grid` | — | B |
| 1 | E5 | Engine: capsule system | `src/engine/CapsuleSystem.cpp` | `tst_capsule_system` | — | C |
| 1 | E6 | Engine: power-up rules | `src/engine/PowerUps.cpp` | `tst_powerups` | — | C |
| 1 | E7 | Engine: weapon system | `src/engine/WeaponSystem.cpp` | `tst_weapon_system` | — | C |
| 1 | E8 | Engine: pace controller (speed + wall descent) | `src/engine/PaceController.cpp` | `tst_pace_controller` | — | B |
| 1 | E9 | Engine: game state machine | `src/engine/GameStateMachine.cpp` | `tst_game_state_machine` | — | A |
| 1 | E10 | Engine: high-score table | `src/engine/HighScoreTable.cpp` | `tst_high_score_table` | — | A |
| 1 | E11 | Engine: list models for QML | `src/engine/Models.cpp` | `tst_models` | — | A |
| 1 | U1 | UI: Brick component | `qml/Brick.qml` | `tst_brick` | — | D |
| 1 | U2 | UI: Paddle component | `qml/Paddle.qml` | `tst_paddle` | — | D |
| 1 | U3 | UI: ball, capsule and projectile sprites | `qml/Ball.qml`, `qml/Capsule.qml`, `qml/Projectile.qml` | `tst_sprites` | — | D |
| 1 | U4 | UI: HUD + pause button | `qml/Hud.qml`, `qml/PauseButton.qml` | `tst_hud` | — | D |
| 1 | U5 | UI: input hint row | `qml/InputHint.qml` | `tst_input_hint` | — | B |
| 1 | U6 | UI: main menu screen | `qml/MenuScreen.qml` | `tst_menu_screen` | — | A |
| 1 | U7 | UI: options screen | `qml/OptionsScreen.qml` | `tst_options_screen` | — | A |
| 1 | U8 | UI: help screen | `qml/HelpScreen.qml` | `tst_help_screen` | — | B |
| 1 | U9 | UI: pause overlay + level banner | `qml/PauseOverlay.qml`, `qml/LevelBanner.qml` | `tst_overlays` | — | C |
| 1 | U10 | UI: game over + initials entry | `qml/InitialsEntry.qml`, `qml/GameOverOverlay.qml` | `tst_game_over` | — | C |
| 1 | U11 | UI: T1 effects layer | `qml/FxLayer.qml` | `tst_fx_layer` | — | D |
| 2 | G1 | Engine: GameEngine core (invokables, input, levels, persistence) | `src/engine/GameEngine.cpp` | `tst_engine_core` | E2, E3, E4, E6, E8, E9, E10, E11 | A |
| 2 | G2 | Engine: ball step (walls, paddle, bricks, losing balls) | `src/engine/GameEngine_balls.cpp` | `tst_engine_balls` | E1, E2, E3, E4, E6, E8, E11 | B |
| 2 | G3 | Engine: power-ups and weapons in the engine | `src/engine/GameEngine_powerups.cpp` | `tst_engine_powerups` | E1, E2, E3, E4, E5, E6, E7, E8 | C |
| 2 | U12 | UI: Board + Playfield | `qml/Board.qml`, `qml/Playfield.qml` | `tst_playfield` | U1, U2, U3 | D |
| 3 | I1 | UI: Game screen (composition + input) | `qml/GameScreen.qml` | `tst_game_screen` | U4, U5, U9, U10, U11, U12 | D |
| 4 | I2 | Integration: AppRoot + Main window + fonts | `qml/AppRoot.qml`, `qml/Main.qml`, `fonts/**` | `tst_app_root` | G1, G2, G3, I1, U6, U7, U8 | A |

Person column = suggested owner (A = integrator). Each person keeps one engine/UI chain so wave 2 builds on code they already know. Wave-1 load: A 5, B 6, C 6, D 5 tasks.

Suggested schedule for 4 days:

- **Day 1:** Wave 1 (≈ 5–6 tasks each; engine and UI tasks mix well).
- **Day 2:** finish Wave 1, then Wave 2 (G1, G2, G3, U12 in parallel).
- **Day 3:** I1, then I2 (Person A), full manual play-through, bug issues.
- **Day 4:** polish and buffer, freeze `main`, tag a release.

## Phase 0 verification

The tests were checked against a private reference implementation (not committed). With it, all 30 ctest tests passed on Qt 6.4 (Ubuntu) in 3 consecutive parallel runs; with the committed stubs, only the 2 sanity tests pass. CI uses Qt 6.8.
