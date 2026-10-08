#pragma once
// OWNER: Phase 0 (interface FROZEN). Score persistence layer, see docs/PERSISTENCE.md.
// Implementations: SqliteScoreStore (local file). A future RemoteScoreStore (server leaderboard)
// implements the same interface, so the engine never changes.
#include <QDateTime>
#include <QString>
#include <QVector>
#include <QtGlobal>

namespace BB {

// Play time is counted in fixed engine steps (K::FixedDt = 1/120 s) and stored in MICROSECONDS.
// Resolution is one step (about 8333 us); the value is exact, deterministic and ignores pauses.
inline qint64 stepsToMicros(qint64 steps) { return steps * 1000000 / 120; }

// One cleared level.
struct LevelClearRecord {
    qint64 id = 0;           // assigned by the store (> 0)
    int level = 0;           // 1..K::LevelCount
    int round = 1;
    int score = 0;           // points earned on this level only
    qint64 timeMicros = 0;   // play time on this level (Playing state only, all lives together)
    QString initials;        // "" when unknown
    QDateTime atUtc;         // invalid -> the store stamps it with "now"
};

// One finished run (game over).
struct RunRecord {
    qint64 id = 0;           // assigned by the store (> 0)
    QString initials;        // "" until the player submits initials
    int score = 0;           // final score of the run
    int level = 1;           // level reached
    int round = 1;
    qint64 timeMicros = 0;   // total play time of the run
    QDateTime atUtc;         // invalid -> the store stamps it with "now"
};

struct LevelBest {
    int bestScore = 0;           // highest LevelClearRecord::score, 0 = never cleared
    qint64 bestTimeMicros = 0;   // lowest LevelClearRecord::timeMicros, 0 = never cleared
    int clears = 0;              // number of records
};

class ScoreStore {
public:
    virtual ~ScoreStore() = default;

    // false when the backing storage could not be opened; every other call is then a harmless no-op
    // (adds return 0 / false, queries return empty / default values).
    virtual bool isOpen() const = 0;

    // Returns the new id (> 0), or 0 on failure.
    virtual qint64 addLevelClear(const LevelClearRecord &r) = 0;
    virtual qint64 addRun(const RunRecord &r) = 0;
    // false when `runId` does not exist.
    virtual bool setRunInitials(qint64 runId, const QString &initials) = 0;

    // Overall leaderboard: score desc, then timeMicros asc, then id asc. At most `limit` (<= 0 -> empty).
    virtual QVector<RunRecord> topRuns(int limit) const = 0;
    // Per-level leaderboards for `level`: by score (score desc, timeMicros asc, id asc) ...
    virtual QVector<LevelClearRecord> topLevelScores(int level, int limit) const = 0;
    // ... and by speed (timeMicros asc, score desc, id asc).
    virtual QVector<LevelClearRecord> fastestLevelTimes(int level, int limit) const = 0;
    virtual LevelBest levelBest(int level) const = 0;
};

} // namespace BB
