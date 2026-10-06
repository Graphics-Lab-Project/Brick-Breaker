#!/usr/bin/env python3
"""
Brick Breaker: create and manage the 28 task issues with the GitHub CLI.

Needs: Python 3.8+, GitHub CLI (`gh`) logged in as a repo admin:
    gh auth login
    gh auth refresh -s project          # only if you use --project / --promote

Usage (run from anywhere; safe to re-run, existing issues are never duplicated):
    python3 tools/create_issues.py --dry-run                 # show what would happen
    python3 tools/create_issues.py --project 1               # labels + 28 issues + board (Wave 1 -> Ready)
    python3 tools/create_issues.py --assign "A=me,B=bob,C=cara,D=dev"   # assign by docs/PLAN.md Person column
    python3 tools/create_issues.py --project 1 --promote     # move Backlog tasks whose blockers are closed -> Ready
    python3 tools/create_issues.py --status                  # table of tasks, state, assignee

Generated from the Phase 0 task list (docs/PLAN.md). OWNER: integrator (Person A).
"""
import argparse
import json
import re
import subprocess
import sys

REPO = "Graphics-Lab-Project/Brick-Breaker"
ORG = "Graphics-Lab-Project"

LABELS = [
    ("task", "1D76DB", "Agent-ready task"),
    ("needs-human", "D93F0B", "Agent is blocked and needs a human answer"),
    ("skip-scope", "BFD4F2", "Non-task PR (Phase 0, docs, CI, test fixes): scope check skipped"),
    ("wave:1", "C2E0C6", "Wave 1: no dependencies"),
    ("wave:2", "BFDADC", "Wave 2"),
    ("wave:3", "FEF2C0", "Wave 3"),
    ("wave:4", "F9D0C4", "Wave 4"),
    ("area:engine", "5319E7", "C++ game logic"),
    ("area:ui", "0E8A16", "QML screens and components"),
    ("area:anim", "FBCA04", "T1 motion / effects"),
]

TASKS = [
 {
  "key": "E1",
  "title": "[E1] Engine: collision helpers",
  "wave": 1,
  "area": "engine",
  "slot": "B",
  "deps": [],
  "body": "**Task E1** · Wave 1 · area:engine\n\n## Goal\nGrid/cell geometry and ball-vs-rect hit-axis rules used by every collision in the game.\n\n## Allowed files\n- src/engine/Collision.cpp\n- ci/green/tst_collision\n\n## Interfaces (frozen)\n- `src/engine/Collision.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_collision.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_collision$\"`\n- When green: create `ci/green/tst_collision` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Implement every function declared in `Collision.h`; its comments are the spec.\n- `cellsOverlapping` uses `Rect::intersects` (strict: touching edges do not count), clips to the 7×14 grid, returns `QPoint(col,row)` row-major.\n- Edge cases tested: four-cell corner overlap, touching edge, board offset, rects outside the grid, zero velocity reflect.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "E2",
  "title": "[E2] Engine: paddle logic",
  "wave": 1,
  "area": "engine",
  "slot": "C",
  "deps": [],
  "body": "**Task E2** · Wave 1 · area:engine\n\n## Goal\nPaddle movement with speed settings 1–5, optional keyboard acceleration ramp, absolute pointer control and clamping.\n\n## Allowed files\n- src/engine/PaddleLogic.cpp\n- ci/green/tst_paddle_logic\n\n## Interfaces (frozen)\n- `src/engine/PaddleLogic.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_paddle_logic.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_paddle_logic$\"`\n- When green: create `ci/green/tst_paddle_logic` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Speeds 120/180/240/300/360 px/s (`K::PaddleSpeeds`). Clamp centre x to `[w/2, 336 − w/2]`.\n- Acceleration: `heldTime += dt` first, then speed × `min(1, heldTime/0.15)`. A direction change restarts the ramp.\n- `motionDir()` = inputDir if non-zero, else the pointer direction recorded since the last `step()` (cleared at the end of `step`).\n- Edge cases tested: dt = 0, width change re-clamps, out-of-range speed setting.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "E3",
  "title": "[E3] Engine: ball physics",
  "wave": 1,
  "area": "engine",
  "slot": "B",
  "deps": [],
  "body": "**Task E3** · Wave 1 · area:engine\n\n## Goal\nPure ball maths: paddle reflection angle by hit position, launch/multi-ball velocities, wall bounces, loss check.\n\n## Allowed files\n- src/engine/BallPhysics.cpp\n- ci/green/tst_ball_physics\n\n## Interfaces (frozen)\n- `src/engine/BallPhysics.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_ball_physics.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_ball_physics$\"`\n- When green: create `ci/green/tst_ball_physics` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Paddle: angle from vertical = clamp(offset/(w/2), −1, 1) × 60°, centre steep, edges shallow. Width ≤ 0 → straight up.\n- Launch: 30° from vertical toward `dir` (dir 0 → right). Multi: −50/−20/20/50°, upward, in that order.\n- Walls: push the centre back inside and make the velocity point away from the wall even if it already did. Bottom is open.\n- `isLost`: top edge (`y − 3`) strictly below 336.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "E4",
  "title": "[E4] Engine: brick grid + 10 levels",
  "wave": 1,
  "area": "engine",
  "slot": "B",
  "deps": [],
  "body": "**Task E4** · Wave 1 · area:engine\n\n## Goal\nThe 7×14 brick grid (load, damage, destroy, counts) and the ten level layouts.\n\n## Allowed files\n- src/engine/BrickGrid.cpp\n- src/engine/Levels.cpp\n- ci/green/tst_brick_grid\n\n## Interfaces (frozen)\n- `src/engine/BrickGrid.h`: everything declared there (signatures, data members, header comments = spec).\n- `src/engine/Levels.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_brick_grid.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_brick_grid$\"`\n- When green: create `ci/green/tst_brick_grid` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Format: ≤ 14 rows × exactly 7 chars from `.123S`; invalid input → `false` and an empty grid.\n- Silver (`S`): alive, unbreakable, hitsLeft 0, tier 0. `hit()` leaves it unchanged; `destroy()` (gun) kills it.\n- Layouts: `docs/DESIGN_HANDOFF.md` §7 (also spelled out in the test). Levels loop: 11 → 1.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "E5",
  "title": "[E5] Engine: capsule system",
  "wave": 1,
  "area": "engine",
  "slot": "C",
  "deps": [],
  "body": "**Task E5** · Wave 1 · area:engine\n\n## Goal\nCapsule drop rolls, falling, catching by the paddle and removal when missed.\n\n## Allowed files\n- src/engine/CapsuleSystem.cpp\n- ci/green/tst_capsule_system\n\n## Interfaces (frozen)\n- `src/engine/CapsuleSystem.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_capsule_system.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_capsule_system$\"`\n- When green: create `ci/green/tst_capsule_system` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Drop: `rand(6) == 0`, then type = `rand(5)`. Only the injected `RandFn` may be used. No RandFn → never drops.\n- Spawn top-left = brick centre − (10, 5). Fall 80 px/s.\n- Caught = strict intersection with the paddle rect (touching is not caught). Lost = `pos.y > 336`.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "E6",
  "title": "[E6] Engine: power-up rules",
  "wave": 1,
  "area": "engine",
  "slot": "C",
  "deps": [],
  "body": "**Task E6** · Wave 1 · area:engine\n\n## Goal\nWhat each capsule does to the paddle mode, gun ammo and lives.\n\n## Allowed files\n- src/engine/PowerUps.cpp\n- ci/green/tst_powerups\n\n## Interfaces (frozen)\n- `src/engine/PowerUps.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_powerups.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_powerups$\"`\n- When green: create `ci/green/tst_powerups` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Life: +1 life (max 9) and ends other powers. Multi: flag only. Long/Gun/Laser replace each other.\n- Gun sets ammo 3 (refill if already Gun, which is not a mode change). Every non-Gun mode sets ammo 0.\n- `resetForLifeLost`: back to Normal, ammo 0, lives untouched.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "E7",
  "title": "[E7] Engine: weapon system",
  "wave": 1,
  "area": "engine",
  "slot": "C",
  "deps": [],
  "body": "**Task E7** · Wave 1 · area:engine\n\n## Goal\nGun bullets and laser bolts: firing rules, positions, cooldown, movement and culling.\n\n## Allowed files\n- src/engine/WeaponSystem.cpp\n- ci/green/tst_weapon_system\n\n## Interfaces (frozen)\n- `src/engine/WeaponSystem.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_weapon_system.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_weapon_system$\"`\n- When green: create `ci/green/tst_weapon_system` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Gun: one bullet at `(paddleX − 1, 312)`, ammo −1, mode → Normal when ammo hits 0, no cooldown.\n- Laser: two bolts at `(paddleX − w/2 + 1, 308)` and `(paddleX + w/2 − 3, 308)`, cooldown 0.25 s.\n- Projectiles move up 400 px/s; removed once their bottom is above y 0. Cooldown never goes negative.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "E8",
  "title": "[E8] Engine: pace controller (speed + wall descent)",
  "wave": 1,
  "area": "engine",
  "slot": "B",
  "deps": [],
  "body": "**Task E8** · Wave 1 · area:engine\n\n## Goal\nSlow → Fast after 50 paddle hits in a life, then the wall descends one row per paddle hit (max 4).\n\n## Allowed files\n- src/engine/PaceController.cpp\n- ci/green/tst_pace_controller\n\n## Interfaces (frozen)\n- `src/engine/PaceController.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_pace_controller.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_pace_controller$\"`\n- When green: create `ci/green/tst_pace_controller` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- The hit that switches to Fast does not descend; descent needs the state to already be Fast.\n- `resetForLife` keeps the descent offset; `resetForLevel` clears it.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "E9",
  "title": "[E9] Engine: game state machine",
  "wave": 1,
  "area": "engine",
  "slot": "A",
  "deps": [],
  "body": "**Task E9** · Wave 1 · area:engine\n\n## Goal\nValidated transitions between Menu / Ready / Playing / Paused / LevelCleared / GameOver.\n\n## Allowed files\n- src/engine/GameStateMachine.cpp\n- ci/green/tst_game_state_machine\n\n## Interfaces (frozen)\n- `src/engine/GameStateMachine.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_game_state_machine.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_game_state_machine$\"`\n- When green: create `ci/green/tst_game_state_machine` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- The transition table is in `GameStateMachine.h`. Everything not listed is rejected (returns false, state unchanged).\n- Pause remembers whether it came from Ready or Playing.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "E10",
  "title": "[E10] Engine: high-score table",
  "wave": 1,
  "area": "engine",
  "slot": "A",
  "deps": [],
  "body": "**Task E10** · Wave 1 · area:engine\n\n## Goal\nTop-5 table with 3-letter initials, persisted through QSettings.\n\n## Allowed files\n- src/engine/HighScoreTable.cpp\n- ci/green/tst_high_score_table\n\n## Interfaces (frozen)\n- `src/engine/HighScoreTable.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_high_score_table.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_high_score_table$\"`\n- When green: create `ci/green/tst_high_score_table` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Initials: trim + uppercase, must be exactly `[A-Z]{3}`. Ties: the new entry goes after existing equal scores.\n- `qualifies`: score > 0 and (table not full or score > lowest).\n- QSettings array `highscores` with keys `initials`/`score`/`level`; `load()` replaces contents.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "E11",
  "title": "[E11] Engine: list models for QML",
  "wave": 1,
  "area": "engine",
  "slot": "A",
  "deps": [],
  "body": "**Task E11** · Wave 1 · area:engine\n\n## Goal\nThe four QAbstractListModels QML renders: bricks, balls, capsules, projectiles.\n\n## Allowed files\n- src/engine/Models.cpp\n- ci/green/tst_models\n\n## Interfaces (frozen)\n- `src/engine/Models.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_models.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_models$\"`\n- When green: create `ci/green/tst_models` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Role names are part of the QML contract: `row col hitsLeft unbreakable alive tier`, `x y`, `type x y`, `kind x y`.\n- Same count → `dataChanged` only (no reset/insert/remove) so QML delegates are not recreated every frame.\n- Tests use `QAbstractItemModelTester`: begin/end calls must be correct.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U1",
  "title": "[U1] UI: Brick component",
  "wave": 1,
  "area": "ui",
  "slot": "D",
  "deps": [],
  "body": "**Task U1** · Wave 1 · area:ui\n\n## Goal\nDraw one brick from `hitsLeft`/`unbreakable`/`alive` with the T1 hit flash.\n\n## Allowed files\n- qml/Brick.qml\n- ci/green/tst_brick\n\n## Interfaces (frozen)\n- `qml/Brick.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_brick.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_brick$\"`\n- When green: create `ci/green/tst_brick` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Tier 3 solid amber, tier 2 solid red, tier 1 hollow red (deep-red fill, 1 px red border), silver grey with a light border.\n- Hidden when not alive. `flash()`: white `hitFlash` overlay 1 → 0 over 60 ms (optional 2 px squash that restores). T1 effects run only when `polish` is true; with `polish: false` the component is static and fully correct.\n- Look: `docs/DESIGN_HANDOFF.md` §3 and §6 and the design PNGs.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U2",
  "title": "[U2] UI: Paddle component",
  "wave": 1,
  "area": "ui",
  "slot": "D",
  "deps": [],
  "body": "**Task U2** · Wave 1 · area:ui\n\n## Goal\nDraw the paddle at `paddleX` with Long width tween, gun turret, laser nubs and the life-lost blink.\n\n## Allowed files\n- qml/Paddle.qml\n- ci/green/tst_paddle\n\n## Interfaces (frozen)\n- `qml/Paddle.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_paddle.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_paddle$\"`\n- When green: create `ci/green/tst_paddle` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- `x = round(paddleX − width/2)`, `y = 322`, height 6. Width tweens 150 ms OutCubic only when polish is on.\n- Gun shows `turret` (6×4, centred, y −4). Laser shows `nubLeft`/`nubRight` (4×4, y −4).\n- `blink()`: opacity 0/1/0/1 in 75 ms steps. T1 effects run only when `polish` is true; with `polish: false` the component is static and fully correct.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U3",
  "title": "[U3] UI: ball, capsule and projectile sprites",
  "wave": 1,
  "area": "ui",
  "slot": "D",
  "deps": [],
  "body": "**Task U3** · Wave 1 · area:ui\n\n## Goal\nThe three small moving sprites.\n\n## Allowed files\n- qml/Ball.qml\n- qml/Capsule.qml\n- qml/Projectile.qml\n- ci/green/tst_sprites\n\n## Interfaces (frozen)\n- `qml/Ball.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- `qml/Capsule.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- `qml/Projectile.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_sprites.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_sprites$\"`\n- When green: create `ci/green/tst_sprites` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Ball: 6×6 at `round(centre − 3)`. Fast trail = `ghost0..2` (opacity 0.55/0.30/0.15), visible only when `fast && polish`.\n- Capsule: 20×10 body in `Theme.capsuleColor`, `label` 8 px text at x 23 with `Theme.capsuleName`. T1 pop-in: scale 0.6 → 1 over 100 ms, OutBack.\n- Projectile: bullet 2×6 amber (`capGun`), laser bolt 2×10 magenta (`capLaser`).\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U4",
  "title": "[U4] UI: HUD + pause button",
  "wave": 1,
  "area": "ui",
  "slot": "D",
  "deps": [],
  "body": "**Task U4** · Wave 1 · area:ui\n\n## Goal\nTop HUD panels (score, best, level, ammo, lives) and the pause button.\n\n## Allowed files\n- qml/Hud.qml\n- qml/PauseButton.qml\n- ci/green/tst_hud\n\n## Interfaces (frozen)\n- `qml/Hud.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- `qml/PauseButton.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_hud.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_hud$\"`\n- When green: create `ci/green/tst_hud` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Score and best padded to 5 digits (`Theme.pad`), best in amber, `LV nn`. Ammo icons are amber up to `gunAmmo`, the rest `ammoEmpty`.\n- `flashLifeLost()`: lives text red → normal over 300 ms. `pulseLifeGained()`: scale 1 → 1.3 → 1 over 200 ms. T1 effects run only when `polish` is true; with `polish: false` the component is static and fully correct.\n- PauseButton: 24×20, two bars, emits `clicked()` on mouse click.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U5",
  "title": "[U5] UI: input hint row",
  "wave": 1,
  "area": "ui",
  "slot": "B",
  "deps": [],
  "body": "**Task U5** · Wave 1 · area:ui\n\n## Goal\nBottom chevron row that lights while input is held, with a key hint between the chevrons.\n\n## Allowed files\n- qml/InputHint.qml\n- ci/green/tst_input_hint\n\n## Interfaces (frozen)\n- `qml/InputHint.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_input_hint.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_input_hint$\"`\n- When green: create `ci/green/tst_input_hint` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Chevron `lit` is `inputDir === ∓1`; colour accent when lit, else `chevronIdle`.\n- Hint: Ready → `SPACE LAUNCH`; Playing with Gun/Laser → `SPACE FIRE`; otherwise empty.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U6",
  "title": "[U6] UI: main menu screen",
  "wave": 1,
  "area": "ui",
  "slot": "A",
  "deps": [],
  "body": "**Task U6** · Wave 1 · area:ui\n\n## Goal\nMain menu with keyboard and mouse selection (mockup 01).\n\n## Allowed files\n- qml/MenuScreen.qml\n- ci/green/tst_menu_screen\n\n## Interfaces (frozen)\n- `qml/MenuScreen.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_menu_screen.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_menu_screen$\"`\n- When green: create `ci/green/tst_menu_screen` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Items START GAME / OPTIONS / HELP / QUIT, wrap-around Up/Down, Return/Enter/Space activates, mouse click activates.\n- `bestText` = `BEST 00830` style. The logo can be drawn with Rectangles (brick row) and Text.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U7",
  "title": "[U7] UI: options screen",
  "wave": 1,
  "area": "ui",
  "slot": "A",
  "deps": [],
  "body": "**Task U7** · Wave 1 · area:ui\n\n## Goal\nPaddle speed (1–5) and acceleration ON/OFF (mockup 06). Requests changes; the parent applies them.\n\n## Allowed files\n- qml/OptionsScreen.qml\n- ci/green/tst_options_screen\n\n## Interfaces (frozen)\n- `qml/OptionsScreen.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_options_screen.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_options_screen$\"`\n- When green: create `ci/green/tst_options_screen` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Rows speed / acceleration / back, Up/Down clamped. Left/Right on speed emit `paddleSpeedRequested(±1)` only if the value changes.\n- Acceleration: Left = ON, Right = OFF (emit only on change), Return toggles. Back row + Return or Escape anywhere → `back()`.\n- The component never changes its own `paddleSpeed`/`acceleration`; the parent does.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U8",
  "title": "[U8] UI: help screen",
  "wave": 1,
  "area": "ui",
  "slot": "B",
  "deps": [],
  "body": "**Task U8** · Wave 1 · area:ui\n\n## Goal\nStatic help: capsule legend, brick legend, scoring, controls (mockup 07).\n\n## Allowed files\n- qml/HelpScreen.qml\n- ci/green/tst_help_screen\n\n## Interfaces (frozen)\n- `qml/HelpScreen.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_help_screen.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_help_screen$\"`\n- When green: create `ci/green/tst_help_screen` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Capsules: LIFE +1 life, ends other powers · MULTI 4 balls · LONG wider paddle · GUN 3 shots, breaks any brick (silver too) · LASER twin bolts, 1 hit each, cannot hurt silver.\n- Scoring: brick hit 10, capsule 50, gun kill 50, laser hit 10. Escape/Return → `back()`.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U9",
  "title": "[U9] UI: pause overlay + level banner",
  "wave": 1,
  "area": "ui",
  "slot": "C",
  "deps": [],
  "body": "**Task U9** · Wave 1 · area:ui\n\n## Goal\nPause menu over the playfield and the LEVEL CLEAR banner (mockups 03, 04).\n\n## Allowed files\n- qml/PauseOverlay.qml\n- qml/LevelBanner.qml\n- ci/green/tst_overlays\n\n## Interfaces (frozen)\n- `qml/PauseOverlay.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- `qml/LevelBanner.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_overlays.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_overlays$\"`\n- When green: create `ci/green/tst_overlays` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Pause: dim ≥ 0.8, title PAUSED, RESUME / OPTIONS / QUIT TO MENU with wrapping keys, Return activates, P/Escape → `resume()`, mouse click activates.\n- Banner: `LEVEL 01`, `CLEAR`, `NEXT LEVEL 02` (after 10 comes 01). `play()`: bar width 0 → 336 over 600 ms OutCubic when polish is on, else 336 immediately.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U10",
  "title": "[U10] UI: game over + initials entry",
  "wave": 1,
  "area": "ui",
  "slot": "C",
  "deps": [],
  "body": "**Task U10** · Wave 1 · area:ui\n\n## Goal\nGame over overlay with the high-score table and 3-letter initials entry (mockup 05).\n\n## Allowed files\n- qml/InitialsEntry.qml\n- qml/GameOverOverlay.qml\n- ci/green/tst_game_over\n\n## Interfaces (frozen)\n- `qml/InitialsEntry.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- `qml/GameOverOverlay.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_game_over.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_game_over$\"`\n- When green: create `ci/green/tst_game_over` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Initials: Up/Down change the letter A–Z with wrap, Left/Right move the cursor (clamped), Return → `submitted(initials)`.\n- Overlay: score padded to 5 digits; when `newBest`, show NEW BEST and the entry (it takes focus) and forward `initialsSubmitted`.\n- Table rows 0–4 from `entries`; empty rows show `---` / `-----` / `--`. Buttons and keys (Return = play again, Escape = menu) when not entering initials. T1: fade in 0 → 0.9 over 400 ms.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U11",
  "title": "[U11] UI: T1 effects layer",
  "wave": 1,
  "area": "anim",
  "slot": "D",
  "deps": [],
  "body": "**Task U11** · Wave 1 · area:anim\n\n## Goal\nShort-lived polish effects spawned from engine signals: brick shards, +50 pop, muzzle flash, multi-split flash.\n\n## Allowed files\n- qml/FxLayer.qml\n- ci/green/tst_fx_layer\n\n## Interfaces (frozen)\n- `qml/FxLayer.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_fx_layer.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_fx_layer$\"`\n- When green: create `ci/green/tst_fx_layer` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- `brickBroken(r,c,tier)` → `breakFx` at `(c·48+1, (r+boardOffsetRows)·24+1)`: scale 1 → 0.6, fade, plus 6 shards (2×2) in `Theme.tierColor(tier)`.\n- `capsuleCaught` → `scorePop` Text `+50` near the paddle, rising 24 px over 500 ms. `projectileFired` → `muzzle`; `multiBallActivated` → `splitFlash` at `ballPosition`.\n- Keep `activeEffects` = number of live effect items; every effect destroys itself (≤ 600 ms). Nothing at all when `polish` is false.\n- Timings: `docs/DESIGN_HANDOFF.md` §6 T1 table.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- none\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "G1",
  "title": "[G1] Engine: GameEngine core (invokables, input, levels, persistence)",
  "wave": 2,
  "area": "engine",
  "slot": "A",
  "deps": [
   "E2",
   "E3",
   "E4",
   "E6",
   "E8",
   "E9",
   "E10",
   "E11"
  ],
  "body": "**Task G1** · Wave 2 · area:engine\n\n## Goal\nEverything QML calls on the engine, plus the input step, the level-clear timer and settings/high-score persistence.\n\n## Allowed files\n- src/engine/GameEngine.cpp\n- ci/green/tst_engine_core\n\n## Interfaces (frozen)\n- `src/engine/GameEngine.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_engine_core.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_engine_core$\"`\n- When green: create `ci/green/tst_engine_core` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Implement the `// task \"Engine core\"` block of `GameEngine.h`. Use the Phase 0 helpers in `GameEngine_loop.cpp`.\n- `startGame` (Menu/GameOver only): score 0, fresh `PowerState`, level 1, round 1, paddle reset, level loaded, ball on paddle; visible without a tick.\n- `launchOrFire`: Ready → launch at `launchVelocity(motionDir or +1, pace speed)`; Playing → `fireWeapon()`. Ready ball follows the paddle (keys and pointer).\n- Level cleared: after 1.5 s load the next level (10 → 1 and round+1), reset powers (keep lives), paddle reset, ball on paddle, `NextLevel`.\n- Persistence through `openSettings()`: keys `paddleSpeed`, `acceleration`, plus `HighScoreTable::load/save`. Missing values → defaults. Save on every change.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- #@@E2@@ [E2] Engine: paddle logic\n- #@@E3@@ [E3] Engine: ball physics\n- #@@E4@@ [E4] Engine: brick grid + 10 levels\n- #@@E6@@ [E6] Engine: power-up rules\n- #@@E8@@ [E8] Engine: pace controller (speed + wall descent)\n- #@@E9@@ [E9] Engine: game state machine\n- #@@E10@@ [E10] Engine: high-score table\n- #@@E11@@ [E11] Engine: list models for QML\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "G2",
  "title": "[G2] Engine: ball step (walls, paddle, bricks, losing balls)",
  "wave": 2,
  "area": "engine",
  "slot": "B",
  "deps": [
   "E1",
   "E2",
   "E3",
   "E4",
   "E6",
   "E8",
   "E11"
  ],
  "body": "**Task G2** · Wave 2 · area:engine\n\n## Goal\n`GameEngine::stepBalls(h)`: move balls, bounce, hit bricks, score, speed-up/descent, lose balls/lives, detect level clear.\n\n## Allowed files\n- src/engine/GameEngine_balls.cpp\n- ci/green/tst_engine_balls\n\n## Interfaces (frozen)\n- `src/engine/GameEngine.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_engine_balls.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_engine_balls$\"`\n- When green: create `ci/green/tst_engine_balls` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Per ball: integrate, `bounceWalls`, then the paddle (moving down + intersects): `paddleBounce` at the speed AFTER `m_pace.onPaddleHit()`, y = 319, `syncPace()`. If it just became Fast, rescale every ball to 260.\n- Bricks: at most one brick per ball per step. Use `cellsOverlapping` + `ballHitAxis`, reflect, step back on the hit axis, `m_grid.hit`, `updateBrick`, +10 unless silver, emit `brickHit`, and if broken emit `brickBroken(r,c,tier)` + call `onBrickBroken`.\n- Remove lost balls; none left → `loseLife()`. Otherwise `checkLevelCleared()`. Never re-implement the Phase 0 helpers.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- #@@E1@@ [E1] Engine: collision helpers\n- #@@E2@@ [E2] Engine: paddle logic\n- #@@E3@@ [E3] Engine: ball physics\n- #@@E4@@ [E4] Engine: brick grid + 10 levels\n- #@@E6@@ [E6] Engine: power-up rules\n- #@@E8@@ [E8] Engine: pace controller (speed + wall descent)\n- #@@E11@@ [E11] Engine: list models for QML\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "G3",
  "title": "[G3] Engine: power-ups and weapons in the engine",
  "wave": 2,
  "area": "engine",
  "slot": "C",
  "deps": [
   "E1",
   "E2",
   "E3",
   "E4",
   "E5",
   "E6",
   "E7",
   "E8"
  ],
  "body": "**Task G3** · Wave 2 · area:engine\n\n## Goal\nCapsule spawn/fall/catch/miss, capsule effects (incl. multi-ball), firing, and projectile hits on bricks.\n\n## Allowed files\n- src/engine/GameEngine_powerups.cpp\n- ci/green/tst_engine_powerups\n\n## Interfaces (frozen)\n- `src/engine/GameEngine.h`: everything declared there (signatures, data members, header comments = spec).\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/engine/tst_engine_powerups.cpp`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_engine_powerups$\"`\n- When green: create `ci/green/tst_engine_powerups` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- `onBrickBroken`: `maybeSpawn` at the cell centre (respect board offset) → `capsuleSpawned(type,x,y)`.\n- `stepPowerUps`: capsules step → caught (+50, `capsuleCaught`, `applyCapsule`) → `capsuleLost` per missed one → weapons step → projectile hits → `checkLevelCleared()`.\n- Projectile hits the alive overlapping cell with the largest row (ties: lowest col). Bullet: `destroy`, +50, `brickBroken`. Laser: silver absorbs it with no effect; else 1 hit, +10, `brickHit` (+ `brickBroken`). The projectile is removed either way. Any broken brick may drop a capsule.\n- `fireWeapon`: `projectileFired(kind,x,y)` per new projectile, then `syncPower()`. Multi: replace all balls with 4 at ball 0's position using `multiBallVelocities(pace speed)`.\n\n## Out of scope\n- QML, other engine files, CMake, tests.\n\n## Blocked by\n- #@@E1@@ [E1] Engine: collision helpers\n- #@@E2@@ [E2] Engine: paddle logic\n- #@@E3@@ [E3] Engine: ball physics\n- #@@E4@@ [E4] Engine: brick grid + 10 levels\n- #@@E5@@ [E5] Engine: capsule system\n- #@@E6@@ [E6] Engine: power-up rules\n- #@@E7@@ [E7] Engine: weapon system\n- #@@E8@@ [E8] Engine: pace controller (speed + wall descent)\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "U12",
  "title": "[U12] UI: Board + Playfield",
  "wave": 2,
  "area": "ui",
  "slot": "D",
  "deps": [
   "U1",
   "U2",
   "U3"
  ],
  "body": "**Task U12** · Wave 2 · area:ui\n\n## Goal\nThe 336×336 clipped playfield rendering the engine models, paddle and the capsule-catch flash.\n\n## Allowed files\n- qml/Board.qml\n- qml/Playfield.qml\n- ci/green/tst_playfield\n\n## Interfaces (frozen)\n- `qml/Board.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- `qml/Playfield.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_playfield.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_playfield$\"`\n- When green: create `ci/green/tst_playfield` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Board: Repeater over `bricks`, Brick at `(col·48+1, row·24+1)`, `y = boardOffsetRows·24` (120 ms slide when polish). `brickAt`, `flashBrick`.\n- Repeaters: capsules (top-left = model x/y, rounded), projectiles, balls (`fast` when `engine.speedState === 1`). Paddle bound to engine.\n- Catch flash: on `capsuleCaught(type)` show `flashTint` (capsule colour, opacity 0.18) and 24 px `flashName`, set `flashType`, hide after 500 ms.\n- Wire T1 hooks (`brickHit` → `flashBrick`, `lifeLost` → `paddle.blink()`) when polish is on. Delegates must use `model.x`/`model.y` (roles shadow Item x/y).\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- #@@U1@@ [U1] UI: Brick component\n- #@@U2@@ [U2] UI: Paddle component\n- #@@U3@@ [U3] UI: ball, capsule and projectile sprites\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "I1",
  "title": "[I1] UI: Game screen (composition + input)",
  "wave": 3,
  "area": "ui",
  "slot": "D",
  "deps": [
   "U4",
   "U5",
   "U9",
   "U10",
   "U11",
   "U12"
  ],
  "body": "**Task I1** · Wave 3 · area:ui\n\n## Goal\nCompose HUD, pause button, playfield, FX, input hint and overlays at the design positions, and route all input to the engine.\n\n## Allowed files\n- qml/GameScreen.qml\n- ci/green/tst_game_screen\n\n## Interfaces (frozen)\n- `qml/GameScreen.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_game_screen.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_game_screen$\"`\n- When green: create `ci/green/tst_game_screen` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- Positions: HUD (8,8), pause button (328,74), hazard strip (8,98, 344×2), frame (8,100, 344×344), playfield (12,104), input hint y 448.\n- Keys: Left/Right press/release → `moveLeft/moveRight(bool)` (ignore auto-repeat), Space → `launchOrFire()`, P/Escape → `togglePause()`.\n- `pointerArea` MouseArea over the playfield (hoverEnabled): position → `setPointerX(x)` in field coords, click → `launchOrFire()`.\n- Overlays visible by `gameState` (3/4/5). The visible overlay takes focus. Pause: resume → `togglePause`, options → `optionsRequested()`, quit → `quitToMenu`. Game over: initials → `submitInitials`, play again → `startGame`, menu → `quitToMenu`.\n- Forward `lifeLost`/`lifeGained` to the HUD effects; call `levelBanner.play()` on `levelCleared`.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- #@@U4@@ [U4] UI: HUD + pause button\n- #@@U5@@ [U5] UI: input hint row\n- #@@U9@@ [U9] UI: pause overlay + level banner\n- #@@U10@@ [U10] UI: game over + initials entry\n- #@@U11@@ [U11] UI: T1 effects layer\n- #@@U12@@ [U12] UI: Board + Playfield\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 },
 {
  "key": "I2",
  "title": "[I2] Integration: AppRoot + Main window + fonts",
  "wave": 4,
  "area": "ui",
  "slot": "A",
  "deps": [
   "G1",
   "G2",
   "G3",
   "I1",
   "U6",
   "U7",
   "U8"
  ],
  "body": "**Task I2** · Wave 4 · area:ui\n**Owner:** Person A\n\n\n## Goal\nWire the real GameEngine into the screens, the 720×960 window and the frame driver. Owned by the integrator (Person A).\n\n## Allowed files\n- qml/AppRoot.qml\n- qml/Main.qml\n- fonts/**\n- ci/green/tst_app_root\n\n## Interfaces (frozen)\n- `qml/AppRoot.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- `qml/Main.qml`: the Phase 0 stub's properties, signals, functions and the listed required `objectName`s.\n- Do not change any interface. If one looks wrong, stop and ask (AGENTS.md section 5).\n\n## Acceptance tests (read-only)\n- File: `tests/qml/tst_app_root.qml`\n- Run: `ctest --test-dir build --output-on-failure -R \"^tst_app_root$\"`\n- When green: create `ci/green/tst_app_root` with content `#<this issue number>` (CI then guards it forever).\n\n## Spec notes\n- AppRoot: menu/help/options/game switching (`screen` + `engine.gameState`); the visible screen holds focus; Options also opens from Pause and returns to the paused game; options requests → `engine.setPaddleSpeed/setAcceleration`; Quit → `Qt.quit()`.\n- Main.qml: fixed 720×960 Window, black, root Item 360×480 with `scale: 2` (TopLeft), `GameEngine { storagePath: \"native\" }`, `FrameAnimation { running: true; onTriggered: engine.tick(frameTime) }`, FontLoaders for `qrc:/fonts/Silkscreen-Regular.ttf` and `-Bold.ttf`.\n- **Human step:** download Silkscreen (OFL) Regular + Bold from Google Fonts into `fonts/` (CMake picks them up automatically).\n- Smoke test by hand: play level 1, catch every capsule type, pause/options, lose all lives, enter initials, restart.\n\n## Out of scope\n- Engine C++, other QML files, CMake, tests.\n\n## Blocked by\n- #@@G1@@ [G1] Engine: GameEngine core (invokables, input, levels, persistence)\n- #@@G2@@ [G2] Engine: ball step (walls, paddle, bricks, losing balls)\n- #@@G3@@ [G3] Engine: power-ups and weapons in the engine\n- #@@I1@@ [I1] UI: Game screen (composition + input)\n- #@@U6@@ [U6] UI: main menu screen\n- #@@U7@@ [U7] UI: options screen\n- #@@U8@@ [U8] UI: help screen\n\n## Reference\n- `docs/PLAN.md` (architecture, gameplay numbers), `docs/DESIGN_HANDOFF.md` (look, motion, levels), `AGENTS.md` (the contract).\n"
 }
]
BY_KEY = {t['key']: t for t in TASKS}


def gh(*args, stdin=None, check=True):
    r = subprocess.run(["gh", *args], input=stdin, capture_output=True, text=True)
    if check and r.returncode != 0:
        sys.exit(f"gh {' '.join(args)}\n  failed: {r.stderr.strip()}")
    return r.stdout.strip()


def existing_issues():
    out = gh("issue", "list", "-R", REPO, "--state", "all", "--limit", "500",
             "--json", "number,title,state,assignees")
    found = {}
    for it in json.loads(out or "[]"):
        m = re.match(r"\[(\w+)\]", it["title"])
        if m and m.group(1) in BY_KEY and m.group(1) not in found:
            found[m.group(1)] = it
    return found


class Board:
    def __init__(self, number):
        self.number = str(number)
        self.id = gh("project", "view", self.number, "--owner", ORG, "--format", "json", "--jq", ".id")
        fields = json.loads(gh("project", "field-list", self.number, "--owner", ORG, "--format", "json", "--limit", "50"))["fields"]
        self.status = next((f for f in fields if f["name"] == "Status"), None)
        self.wave = next((f for f in fields if f["name"] == "Wave"), None)
        self.area = next((f for f in fields if f["name"] == "Area"), None)
        if not self.status:
            sys.exit("Project has no 'Status' field.")
        need = {"Backlog", "Ready"}
        have = {o["name"] for o in self.status.get("options", [])}
        if not need <= have:
            sys.exit(f"Status field needs options {sorted(need)}; has {sorted(have)}")

    def option(self, field, name):
        return next((o["id"] for o in field.get("options", []) if o["name"].lower() == name.lower()), None)

    def add(self, url):
        return gh("project", "item-add", self.number, "--owner", ORG, "--url", url, "--format", "json", "--jq", ".id")

    def set_status(self, item_id, name):
        gh("project", "item-edit", "--id", item_id, "--project-id", self.id,
           "--field-id", self.status["id"], "--single-select-option-id", self.option(self.status, name))

    def set_fields(self, item_id, task):
        if self.wave and self.wave.get("type", "").endswith("Field") and "options" not in self.wave:
            gh("project", "item-edit", "--id", item_id, "--project-id", self.id,
               "--field-id", self.wave["id"], "--number", str(task["wave"]), check=False)
        if self.area and "options" in self.area:
            opt = self.option(self.area, task["area"])
            if opt:
                gh("project", "item-edit", "--id", item_id, "--project-id", self.id,
                   "--field-id", self.area["id"], "--single-select-option-id", opt, check=False)

    def items(self):
        out = json.loads(gh("project", "item-list", self.number, "--owner", ORG, "--format", "json", "--limit", "500"))
        return out.get("items", [])


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--dry-run", action="store_true", help="print actions, change nothing")
    ap.add_argument("--project", type=int, help="org Project number (from its URL) to add issues to")
    ap.add_argument("--assign", help='slot to GitHub handle, e.g. "A=me,B=bob,C=cara,D=dev"')
    ap.add_argument("--promote", action="store_true", help="Backlog -> Ready when all blockers are closed (needs --project)")
    ap.add_argument("--status", action="store_true", help="print a status table and exit")
    a = ap.parse_args()

    gh("auth", "status")
    found = existing_issues()

    if a.status:
        for t in TASKS:
            it = found.get(t["key"])
            who = ",".join(x["login"] for x in it["assignees"]) if it else ""
            print(f"{t['key']:>4}  w{t['wave']}  {t['slot']}  " +
                  (f"#{it['number']:<4} {it['state']:<6} {who:<16}" if it else "(not created)".ljust(30)) + t["title"])
        return

    if not a.dry_run:
        for name, color, desc in LABELS:
            gh("label", "create", name, "-R", REPO, "--color", color, "--description", desc, "--force")
        print(f"labels: {len(LABELS)} ensured")

    board = Board(a.project) if (a.project and not a.dry_run) else None
    numbers = {k: v["number"] for k, v in found.items()}

    for t in TASKS:                       # topological order: blockers are created first
        if t["key"] in numbers:
            continue
        missing = [d for d in t["deps"] if d not in numbers]
        if missing and not a.dry_run:
            sys.exit(f"{t['key']}: blockers {missing} have no issue yet")
        body = t["body"]
        for k, n in numbers.items():
            body = body.replace(f"@@{k}@@", str(n))
        labels = ["task", f"wave:{t['wave']}", f"area:{t['area']}"]
        if a.dry_run:
            numbers[t["key"]] = f"<{t['key']}>"
            print(f"would create {t['title']}  labels={labels}  blocked_by={t['deps'] or '-'}")
            continue
        url = gh("issue", "create", "-R", REPO, "--title", t["title"], "--body-file", "-",
                 *sum((["--label", l] for l in labels), []), stdin=body)
        num = int(url.rstrip("/").split("/")[-1])
        numbers[t["key"]] = num
        print(f"created #{num} {t['title']}")
        if board:
            item = board.add(url)
            board.set_status(item, "Ready" if t["wave"] == 1 else "Backlog")
            board.set_fields(item, t)

    if a.assign:
        slots = dict(p.split("=", 1) for p in a.assign.replace(" ", "").split(",") if "=" in p)
        for t in TASKS:
            handle = slots.get(t["slot"], "").lstrip("@")
            if not handle:
                continue
            if a.dry_run:
                print(f"would assign {t['key']} -> @{handle}")
            else:
                gh("issue", "edit", str(numbers[t["key"]]), "-R", REPO, "--add-assignee", handle)
                print(f"assigned #{numbers[t['key']]} {t['key']} -> @{handle}")

    if a.promote:
        if not a.project:
            sys.exit("--promote needs --project")
        if a.dry_run:
            print("would promote unblocked Backlog items to Ready")
            return
        board = board or Board(a.project)
        state = {k: v["state"] for k, v in existing_issues().items()}
        by_num = {v: k for k, v in numbers.items()}
        for item in board.items():
            num = (item.get("content") or {}).get("number")
            key = by_num.get(num)
            if not key or (item.get("status") or "") != "Backlog":
                continue
            deps = BY_KEY[key]["deps"]
            if all(state.get(d) == "CLOSED" for d in deps):
                board.set_status(item["id"], "Ready")
                print(f"promoted #{num} {key} -> Ready")

    print("done.")


if __name__ == "__main__":
    main()
