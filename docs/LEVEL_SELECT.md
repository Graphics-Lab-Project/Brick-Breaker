# Level select (feature plan)

**Goal.** START GAME / CONTINUE opens a scrollable level list. Levels are locked until the one before
them has been cleared. Progress is saved. A dev switch unlocks everything.

## Behaviour
- Menu item 0 reads **START GAME** on first launch (`engine.hasProgress == false`) and **CONTINUE** afterwards.
  Both open the level select (no game starts from the menu any more).
- Level select: `LEVEL 01..20`, one row each, in a vertical `Flickable` (mouse wheel/drag, or Up/Down keys; the
  selected row is scrolled into view). Opens with the highest unlocked level selected.
  Locked rows are dimmed and show `LOCKED`; they can be selected but never started.
  Return/Enter/Space or a click on an unlocked row starts it; Esc or BACK returns to the menu.
- Engine: `startLevel(n)` = `startGame()` at level `n` (round 1, score 0, lives 3). `startGame()` stays and equals
  `startLevel(1)`. After a start the game proceeds as before (clear level n -> n+1 ...).
- Unlock rule: clearing level n unlocks n+1 (capped at `K::LevelCount` = 20). Done in `enterLevelCleared()` so quitting during the
  clear banner still keeps it. Never lowers. Saved with the other settings (`unlockedLevel`, `hasProgress`).
- Dev: `GameEngine.unlockAll` (not persisted, default false). `qml/Main.qml` sets it to `true` for now;
  delete that one line to ship with real locking.

## Interfaces added (frozen, see the headers/stubs)
`GameEngine`: `unlockedLevel`, `hasProgress`, `unlockAll` properties; `startLevel(int)`; private `unlockThrough(int)`.
`LevelSelectScreen.qml` (new). `MenuScreen.hasProgress`. `AppRoot` screen `"levels"` + child `levelSelectScreen`.

## Tasks and tests
| Task | Files | Test |
|---|---|---|
| L1 engine | `src/engine/GameEngine.cpp` | `tst_engine_levels` |
| L2 screen | `qml/LevelSelectScreen.qml` | `tst_level_select` |
| L3 wiring | `qml/MenuScreen.qml`, `qml/AppRoot.qml` | `tst_level_flow` (+ `tst_menu_screen`, `tst_app_root` stay green) |

## 20 levels
`K::LevelCount` = 20, exposed to QML as `engine.levelCount`. Levels 1-8 unchanged; 9 and 10 reduced to 4 stones (`S`)
each (the old ones left the player stuck); 11-20 are new shapes (Invader, Heart, Pine, Rocket, Zigzag, Hourglass, Butterfly,
Bullseye, Crown, Skull). Rule: at most 4 `S` and at most 9 rows per level (tested). Layouts: `tests/engine/tst_brick_grid.cpp`.
Tasks: M1 `Levels.cpp` (`tst_brick_grid`), M2 banner wrap + list size wiring (`tst_level_flow`).
