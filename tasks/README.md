# Tasks

31 tasks (28 original + 3 level-select tasks, see `docs/LEVEL_SELECT.md`), one spec file each (`tasks/<KEY>.md`). Every task owns different files, so tasks in the
same wave can run in parallel. Keys: **E** engine module · **U** UI component · **G** engine
integration · **I** final integration.

- **Done** = its acceptance test passes. Check: `python3 scripts/task_status.py`.
- **Ready** = not done, and every task in "Depends on" is done.
- **Model** = which coder subagent gets it: `haiku` → `task-coder-fast`, `sonnet` → `task-coder`.
- **Tests:** engine tasks → `tests/engine/<test>.cpp` (QtTest); UI tasks → `tests/qml/<test>.qml`
  (Qt Quick Test, with `tests/qml/MockEngine.qml` standing in for the engine). Read-only.
- **Ledger:** `tasks/LEDGER.md`, written by the orchestrator (dispatches, attempts, rulings).

| Wave | Key | Task | Model | Depends on | Test | Owns |
|---|---|---|---|---|---|---|
| 1 | [E1](E1.md) | Engine: collision helpers | haiku | none | `tst_collision` | `src/engine/Collision.cpp` |
| 1 | [E2](E2.md) | Engine: paddle logic | haiku | none | `tst_paddle_logic` | `src/engine/PaddleLogic.cpp` |
| 1 | [E3](E3.md) | Engine: ball physics | haiku | none | `tst_ball_physics` | `src/engine/BallPhysics.cpp` |
| 1 | [E4](E4.md) | Engine: brick grid + 10 levels | haiku | none | `tst_brick_grid` | `src/engine/BrickGrid.cpp`, `src/engine/Levels.cpp` |
| 1 | [E5](E5.md) | Engine: capsule system | haiku | none | `tst_capsule_system` | `src/engine/CapsuleSystem.cpp` |
| 1 | [E6](E6.md) | Engine: power-up rules | haiku | none | `tst_powerups` | `src/engine/PowerUps.cpp` |
| 1 | [E7](E7.md) | Engine: weapon system | haiku | none | `tst_weapon_system` | `src/engine/WeaponSystem.cpp` |
| 1 | [E8](E8.md) | Engine: pace controller (speed + wall descent) | haiku | none | `tst_pace_controller` | `src/engine/PaceController.cpp` |
| 1 | [E9](E9.md) | Engine: game state machine | haiku | none | `tst_game_state_machine` | `src/engine/GameStateMachine.cpp` |
| 1 | [E10](E10.md) | Engine: high-score table | haiku | none | `tst_high_score_table` | `src/engine/HighScoreTable.cpp` |
| 1 | [E11](E11.md) | Engine: list models for QML | sonnet | none | `tst_models` | `src/engine/Models.cpp` |
| 1 | [U1](U1.md) | UI: Brick component | sonnet | none | `tst_brick` | `qml/Brick.qml` |
| 1 | [U2](U2.md) | UI: Paddle component | sonnet | none | `tst_paddle` | `qml/Paddle.qml` |
| 1 | [U3](U3.md) | UI: ball, capsule and projectile sprites | haiku | none | `tst_sprites` | `qml/Ball.qml`, `qml/Capsule.qml`, `qml/Projectile.qml` |
| 1 | [U4](U4.md) | UI: HUD + pause button | sonnet | none | `tst_hud` | `qml/Hud.qml`, `qml/PauseButton.qml` |
| 1 | [U5](U5.md) | UI: input hint row | haiku | none | `tst_input_hint` | `qml/InputHint.qml` |
| 1 | [U6](U6.md) | UI: main menu screen | sonnet | none | `tst_menu_screen` | `qml/MenuScreen.qml` |
| 1 | [U7](U7.md) | UI: options screen | sonnet | none | `tst_options_screen` | `qml/OptionsScreen.qml` |
| 1 | [U8](U8.md) | UI: help screen | haiku | none | `tst_help_screen` | `qml/HelpScreen.qml` |
| 1 | [U9](U9.md) | UI: pause overlay + level banner | sonnet | none | `tst_overlays` | `qml/PauseOverlay.qml`, `qml/LevelBanner.qml` |
| 1 | [U10](U10.md) | UI: game over + initials entry | sonnet | none | `tst_game_over` | `qml/InitialsEntry.qml`, `qml/GameOverOverlay.qml` |
| 1 | [U11](U11.md) | UI: T1 effects layer | sonnet | none | `tst_fx_layer` | `qml/FxLayer.qml` |
| 2 | [G1](G1.md) | Engine: GameEngine core (invokables, input, levels, persistence) | sonnet | E2, E3, E4, E6, E8, E9, E10, E11 | `tst_engine_core` | `src/engine/GameEngine.cpp` |
| 2 | [G2](G2.md) | Engine: ball step (walls, paddle, bricks, losing balls) | sonnet | E1, E2, E3, E4, E6, E8, E11 | `tst_engine_balls` | `src/engine/GameEngine_balls.cpp` |
| 2 | [G3](G3.md) | Engine: power-ups and weapons in the engine | sonnet | E1, E2, E3, E4, E5, E6, E7, E8 | `tst_engine_powerups` | `src/engine/GameEngine_powerups.cpp` |
| 2 | [U12](U12.md) | UI: Board + Playfield | sonnet | U1, U2, U3 | `tst_playfield` | `qml/Board.qml`, `qml/Playfield.qml` |
| 3 | [I1](I1.md) | UI: Game screen (composition + input) | sonnet | U4, U5, U9, U10, U11, U12 | `tst_game_screen` | `qml/GameScreen.qml` |
| 4 | [I2](I2.md) | Integration: AppRoot + Main window + fonts | sonnet | G1, G2, G3, I1, U6, U7, U8 | `tst_app_root` | `qml/AppRoot.qml`, `qml/Main.qml`, `fonts/**` |
| 1 | [L1](L1.md) | Engine: level select (unlock, progress, startLevel) | sonnet | none | `tst_engine_levels` | `src/engine/GameEngine.cpp` |
| 1 | [L2](L2.md) | UI: level select screen | sonnet | none | `tst_level_select` | `qml/LevelSelectScreen.qml` |
| 2 | [L3](L3.md) | UI: menu CONTINUE label and level select wiring | sonnet | L1, L2 | `tst_level_flow` | `qml/MenuScreen.qml`, `qml/AppRoot.qml` |
