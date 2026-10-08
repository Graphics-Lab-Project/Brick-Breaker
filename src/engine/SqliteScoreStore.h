#pragma once
// OWNER: Phase 0 (interface + data members FROZEN). Implementation: task "Sqlite score store" -> SqliteScoreStore.cpp
#include "ScoreStore.h"

namespace BB {

// Local SQLite file (QtSql, driver QSQLITE). Schema (PRAGMA user_version = 1), one row per record:
//   runs(id INTEGER PRIMARY KEY AUTOINCREMENT, initials TEXT NOT NULL, score INTEGER NOT NULL,
//        level INTEGER NOT NULL, round INTEGER NOT NULL, time_us INTEGER NOT NULL, at_utc TEXT NOT NULL)
//   level_clears(id INTEGER PRIMARY KEY AUTOINCREMENT, level INTEGER NOT NULL, round INTEGER NOT NULL,
//        score INTEGER NOT NULL, time_us INTEGER NOT NULL, initials TEXT NOT NULL, at_utc TEXT NOT NULL)
// plus indexes for the leaderboard queries. at_utc = ISO-8601 with milliseconds ("2026-01-02T03:04:05.678Z").
// time_us is a 64-bit integer of microseconds. The tables map 1:1 onto a server-side SQL schema.
class SqliteScoreStore final : public ScoreStore {
public:
    // `path` = database file (missing parent directories are created) or ":memory:".
    // Each instance uses its own uniquely named QSqlDatabase connection, removed in the destructor.
    explicit SqliteScoreStore(const QString &path);
    ~SqliteScoreStore() override;

    bool isOpen() const override;
    qint64 addLevelClear(const LevelClearRecord &r) override;
    qint64 addRun(const RunRecord &r) override;
    bool setRunInitials(qint64 runId, const QString &initials) override;
    QVector<RunRecord> topRuns(int limit) const override;
    QVector<LevelClearRecord> topLevelScores(int level, int limit) const override;
    QVector<LevelClearRecord> fastestLevelTimes(int level, int limit) const override;
    LevelBest levelBest(int level) const override;

private:
    QString m_connection;   // QSqlDatabase connection name, unique per instance
    bool m_open = false;
};

} // namespace BB
