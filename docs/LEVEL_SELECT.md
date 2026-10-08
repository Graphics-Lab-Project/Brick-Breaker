# Level select (feature plan)

**Goal.** START GAME / CONTINUE opens a scrollable level list. Levels are locked until the one before
them has been cleared. Progress is saved. A dev switch unlocks everything.

## Behaviour
- Menu item 0 reads **START GAME** on first launch (`engine.hasProgress == false`) and **CONTINUE** afterwards.
  Both open the level select (no game starts from the menu any more).
- Level select: `LEVEL 01..10`, one row each, in a vertical `Flickable` (mouse wheel/drag, or Up/Down keys; the
  selected row is scrolled into view). Opens with the highest unlocked level selected.
  Locked rows are dimmed and show `LOCKED`; they can be selected but never started.
  Return/Enter/Space or a click on an unlocked row starts it; Esc or BACK returns to the menu.
- Engine: `startLevel(n)` = `startGame()` at level `n` (round 1, score 0, lives 3). `startGame()` stays and equals
  `startLevel(1)`. After a start the game proceeds as before (clear level n -> n+1 ...).
- Unlock rule: clearing level n unlocks n+1 (capped at 10). Done in `enterLevelCleared()` so quitting during the
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
