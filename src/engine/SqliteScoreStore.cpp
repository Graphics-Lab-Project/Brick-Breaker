// OWNER: task "Sqlite score store".
#include "SqliteScoreStore.h"

#include <QDir>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <atomic>

namespace BB {

namespace {

std::atomic<int> g_counter{0};

// An empty QString may be null, which would bind as SQL NULL; the column is NOT NULL.
QString nonNull(const QString &s) { return s.isNull() ? QStringLiteral("") : s; }

QString toIso(const QDateTime &dt)
{
    QDateTime t = dt.isValid() ? dt : QDateTime::currentDateTimeUtc();
    return t.toUTC().toString(Qt::ISODateWithMs);
}

QDateTime fromIso(const QString &s)
{
    QDateTime t = QDateTime::fromString(s, Qt::ISODateWithMs);
    if (t.isValid())
        t.setTimeSpec(Qt::UTC);
    return t;
}

LevelClearRecord readClear(const QSqlQuery &q)
{
    LevelClearRecord r;
    r.id = q.value(0).toLongLong();
    r.level = q.value(1).toInt();
    r.round = q.value(2).toInt();
    r.score = q.value(3).toInt();
    r.timeMicros = q.value(4).toLongLong();
    r.initials = q.value(5).toString();
    r.atUtc = fromIso(q.value(6).toString());
    return r;
}

} // namespace

SqliteScoreStore::SqliteScoreStore(const QString &path)
{
    m_connection = QStringLiteral("bb_scores_%1_%2").arg(quintptr(this)).arg(g_counter.fetch_add(1));
    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE")))
        return;
    if (path != QLatin1String(":memory:"))
        QDir().mkpath(QFileInfo(path).absolutePath());

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connection);
    db.setDatabaseName(path);
    if (!db.open())
        return;

    QSqlQuery q(db);
    bool ok = true;
    ok = ok && q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS runs(id INTEGER PRIMARY KEY AUTOINCREMENT, initials TEXT NOT NULL, "
        "score INTEGER NOT NULL, level INTEGER NOT NULL, round INTEGER NOT NULL, "
        "time_us INTEGER NOT NULL, at_utc TEXT NOT NULL)"));
    ok = ok && q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS level_clears(id INTEGER PRIMARY KEY AUTOINCREMENT, level INTEGER NOT NULL, "
        "round INTEGER NOT NULL, score INTEGER NOT NULL, time_us INTEGER NOT NULL, "
        "initials TEXT NOT NULL, at_utc TEXT NOT NULL)"));
    ok = ok && q.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_runs_rank ON runs(score DESC, time_us ASC, id ASC)"));
    ok = ok && q.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_lc_score ON level_clears(level, score DESC, time_us ASC, id ASC)"));
    ok = ok && q.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_lc_time ON level_clears(level, time_us ASC, score DESC, id ASC)"));
    if (ok) {
        q.exec(QStringLiteral("PRAGMA user_version"));
        if (q.next() && q.value(0).toInt() == 0)
            q.exec(QStringLiteral("PRAGMA user_version = 1"));
    }
    m_open = ok;
}

SqliteScoreStore::~SqliteScoreStore()
{
    {
        QSqlDatabase db = QSqlDatabase::database(m_connection, false);
        if (db.isValid() && db.isOpen())
            db.close();
    }
    QSqlDatabase::removeDatabase(m_connection);
}

bool SqliteScoreStore::isOpen() const { return m_open; }

qint64 SqliteScoreStore::addLevelClear(const LevelClearRecord &r)
{
    if (!m_open)
        return 0;
    QSqlDatabase db = QSqlDatabase::database(m_connection, false);
    QSqlQuery q(db);
    q.prepare(QStringLiteral("INSERT INTO level_clears(level, round, score, time_us, initials, at_utc) "
                             "VALUES(?,?,?,?,?,?)"));
    q.addBindValue(r.level);
    q.addBindValue(r.round);
    q.addBindValue(r.score);
    q.addBindValue(qlonglong(r.timeMicros));
    q.addBindValue(nonNull(r.initials));
    q.addBindValue(toIso(r.atUtc));
    if (!q.exec())
        return 0;
    return q.lastInsertId().toLongLong();
}

qint64 SqliteScoreStore::addRun(const RunRecord &r)
{
    if (!m_open)
        return 0;
    QSqlDatabase db = QSqlDatabase::database(m_connection, false);
    QSqlQuery q(db);
    q.prepare(QStringLiteral("INSERT INTO runs(initials, score, level, round, time_us, at_utc) "
                             "VALUES(?,?,?,?,?,?)"));
    q.addBindValue(nonNull(r.initials));
    q.addBindValue(r.score);
    q.addBindValue(r.level);
    q.addBindValue(r.round);
    q.addBindValue(qlonglong(r.timeMicros));
    q.addBindValue(toIso(r.atUtc));
    if (!q.exec())
        return 0;
    return q.lastInsertId().toLongLong();
}

bool SqliteScoreStore::setRunInitials(qint64 runId, const QString &initials)
{
    if (!m_open)
        return false;
    QSqlDatabase db = QSqlDatabase::database(m_connection, false);
    QSqlQuery q(db);
    q.prepare(QStringLiteral("UPDATE runs SET initials = ? WHERE id = ?"));
    q.addBindValue(nonNull(initials));
    q.addBindValue(qlonglong(runId));
    return q.exec() && q.numRowsAffected() > 0;
}

QVector<RunRecord> SqliteScoreStore::topRuns(int limit) const
{
    QVector<RunRecord> out;
    if (!m_open || limit <= 0)
        return out;
    QSqlDatabase db = QSqlDatabase::database(m_connection, false);
    QSqlQuery q(db);
    q.prepare(QStringLiteral("SELECT id, initials, score, level, round, time_us, at_utc FROM runs "
                             "ORDER BY score DESC, time_us ASC, id ASC LIMIT ?"));
    q.addBindValue(limit);
    if (!q.exec())
        return out;
    while (q.next()) {
        RunRecord r;
        r.id = q.value(0).toLongLong();
        r.initials = q.value(1).toString();
        r.score = q.value(2).toInt();
        r.level = q.value(3).toInt();
        r.round = q.value(4).toInt();
        r.timeMicros = q.value(5).toLongLong();
        r.atUtc = fromIso(q.value(6).toString());
        out.push_back(r);
    }
    return out;
}

static QVector<LevelClearRecord> queryClears(const QString &connection, bool m_open, const QString &order,
                                             int level, int limit)
{
    QVector<LevelClearRecord> out;
    if (!m_open || limit <= 0)
        return out;
    QSqlDatabase db = QSqlDatabase::database(connection, false);
    QSqlQuery q(db);
    q.prepare(QStringLiteral("SELECT id, level, round, score, time_us, initials, at_utc FROM level_clears "
                             "WHERE level = ? ORDER BY ") + order + QStringLiteral(" LIMIT ?"));
    q.addBindValue(level);
    q.addBindValue(limit);
    if (!q.exec())
        return out;
    while (q.next())
        out.push_back(readClear(q));
    return out;
}

QVector<LevelClearRecord> SqliteScoreStore::topLevelScores(int level, int limit) const
{
    return queryClears(m_connection, m_open, QStringLiteral("score DESC, time_us ASC, id ASC"), level, limit);
}

QVector<LevelClearRecord> SqliteScoreStore::fastestLevelTimes(int level, int limit) const
{
    return queryClears(m_connection, m_open, QStringLiteral("time_us ASC, score DESC, id ASC"), level, limit);
}

LevelBest SqliteScoreStore::levelBest(int level) const
{
    LevelBest b;
    if (!m_open)
        return b;
    QSqlDatabase db = QSqlDatabase::database(m_connection, false);
    QSqlQuery q(db);
    q.prepare(QStringLiteral("SELECT MAX(score), MIN(time_us), COUNT(*) FROM level_clears WHERE level = ?"));
    q.addBindValue(level);
    if (q.exec() && q.next()) {
        b.clears = q.value(2).toInt();
        if (b.clears > 0) {
            b.bestScore = q.value(0).toInt();
            b.bestTimeMicros = q.value(1).toLongLong();
        }
    }
    return b;
}

} // namespace BB
