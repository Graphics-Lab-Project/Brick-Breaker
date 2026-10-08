# Score persistence (feature plan)

**Goal.** High scores survive restarts, per level and for the whole game, with the time it took
(microseconds), behind an interface that a server-side SQL leaderboard can implement later.

## What is stored
- **Level clear** (`LevelClearRecord`), one per cleared level: level, round, points earned on that level, play time
  on that level, initials ("" for now: per-level records have no name prompt), UTC timestamp.
- **Run** (`RunRecord`), one per game over: final score, level reached, round, total play time of the run,
  initials (empty until the player submits them: `submitInitials` updates the same record), UTC timestamp.
- Time is **play time**: only fixed engine steps taken while the state is Playing (not Ready, Paused or the clear banner),
  all lives of a level together. Stored in microseconds as a 64-bit integer (`stepsToMicros`, 1 step = 1/120 s = 8333.33 us),
  so it is exact and replay-deterministic; its resolution is one step. (A wall-clock timer would not be deterministic
  and would include pauses.)

## Layers
```
GameEngine  --(ScoreStore interface, ScoreStore.h)-->  SqliteScoreStore (local file)      <- now
                                                  \->  RemoteScoreStore (HTTP / SQL server) <- later
QML: engine.levelBests / levelBest(n) / levelScores(n) / levelTimes(n) / runScores()
```
- `ScoreStore` is a plain C++ interface (add, top-N queries, per-level best). The engine never talks SQL.
- `SqliteScoreStore`: QtSql (QSQLITE), tables `runs` and `level_clears` with `time_us INTEGER` and ISO-8601 UTC
  timestamps, `PRAGMA user_version = 1` for migrations. The columns map 1:1 onto a server table.
- File: `setStoragePath("native")` -> `<AppDataLocation>/scores.sqlite`; an INI path `X` -> `X.scores.sqlite`;
  `""` -> no store (tests, nothing persisted). `GameEngine::setScoreStore()` injects any other implementation.
- The old top-5 initials table (`HighScoreTable`, QSettings) is unchanged and still drives the game-over screen.

## UI
Level select rows show the personal best of cleared levels: `BEST 01240` and `0:42.318457` (M:SS.uuuuuu).

## Tasks
| Task | Files | Test |
|---|---|---|
| S1 | `src/engine/SqliteScoreStore.cpp` | `tst_score_store` |
| S2 | `src/engine/GameEngine.cpp` | `tst_engine_scores` (needs S1) |
| S3 | `qml/LevelSelectScreen.qml`, `qml/AppRoot.qml` | `tst_level_select` |
