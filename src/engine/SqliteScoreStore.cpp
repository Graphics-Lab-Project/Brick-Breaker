// OWNER: task "Sqlite score store". Phase 0 stub: compiles, behaviour missing.
#include "SqliteScoreStore.h"

namespace BB {

SqliteScoreStore::SqliteScoreStore(const QString &) {}
SqliteScoreStore::~SqliteScoreStore() {}
bool SqliteScoreStore::isOpen() const { return false; }
qint64 SqliteScoreStore::addLevelClear(const LevelClearRecord &) { return 0; }
qint64 SqliteScoreStore::addRun(const RunRecord &) { return 0; }
bool SqliteScoreStore::setRunInitials(qint64, const QString &) { return false; }
QVector<RunRecord> SqliteScoreStore::topRuns(int) const { return {}; }
QVector<LevelClearRecord> SqliteScoreStore::topLevelScores(int, int) const { return {}; }
QVector<LevelClearRecord> SqliteScoreStore::fastestLevelTimes(int, int) const { return {}; }
LevelBest SqliteScoreStore::levelBest(int) const { return {}; }

} // namespace BB
