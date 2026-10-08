// Acceptance tests for task "Sqlite score store" (SqliteScoreStore.cpp). READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "SqliteScoreStore.h"
#include "Constants.h"
#include <QTemporaryDir>

using namespace BB;

namespace {
LevelClearRecord clear(int level, int score, qint64 timeUs, const QString &initials = QString())
{
    LevelClearRecord r;
    r.level = level;
    r.score = score;
    r.timeMicros = timeUs;
    r.initials = initials;
    return r;
}
RunRecord run(int score, qint64 timeUs, int level = 1, const QString &initials = QString())
{
    RunRecord r;
    r.score = score;
    r.timeMicros = timeUs;
    r.level = level;
    r.initials = initials;
    return r;
}
}

class TstScoreStore : public QObject {
    Q_OBJECT
private slots:
    void stepsToMicros_isExact()
    {
        QCOMPARE(stepsToMicros(0), qint64(0));
        QCOMPARE(stepsToMicros(120), qint64(1000000));
        QCOMPARE(stepsToMicros(60), qint64(500000));
        QCOMPARE(stepsToMicros(1), qint64(8333));
        QCOMPARE(1.0 / K::FixedDt, 120.0);
    }
    void opensInMemory()
    {
        SqliteScoreStore s(":memory:");
        QVERIFY(s.isOpen());
        QCOMPARE(s.levelBest(1).clears, 0);
        QVERIFY(s.topRuns(5).isEmpty());
        QVERIFY(s.topLevelScores(1, 5).isEmpty());
        QVERIFY(s.fastestLevelTimes(1, 5).isEmpty());
    }
    void levelClear_roundTripKeepsMicroseconds()
    {
        SqliteScoreStore s(":memory:");
        LevelClearRecord r = clear(3, 1240, 42318457, "BNY");
        r.round = 2;
        r.atUtc = QDateTime::fromString("2026-01-02T03:04:05.678Z", Qt::ISODateWithMs);
        QVERIFY(r.atUtc.isValid());
        const qint64 id = s.addLevelClear(r);
        QVERIFY(id > 0);
        const auto got = s.topLevelScores(3, 5);
        QCOMPARE(got.size(), 1);
        QCOMPARE(got[0].id, id);
        QCOMPARE(got[0].level, 3);
        QCOMPARE(got[0].round, 2);
        QCOMPARE(got[0].score, 1240);
        QCOMPARE(got[0].timeMicros, qint64(42318457));
        QCOMPARE(got[0].initials, QStringLiteral("BNY"));
        QCOMPARE(got[0].atUtc.toMSecsSinceEpoch(), r.atUtc.toMSecsSinceEpoch());
        QCOMPARE(got[0].atUtc.timeSpec(), Qt::UTC);
    }
    void hugeTimeFitsInt64()
    {
        SqliteScoreStore s(":memory:");
        const qint64 big = 4000000000000LL;          // > 2^32 microseconds
        QVERIFY(s.addLevelClear(clear(1, 10, big)) > 0);
        QCOMPARE(s.levelBest(1).bestTimeMicros, big);
    }
    void invalidTimestampIsStamped()
    {
        SqliteScoreStore s(":memory:");
        s.addRun(run(100, 1000));
        const auto runs = s.topRuns(1);
        QCOMPARE(runs.size(), 1);
        QVERIFY(runs[0].atUtc.isValid());
        QVERIFY(qAbs(runs[0].atUtc.secsTo(QDateTime::currentDateTimeUtc())) < 60);
    }
    void idsIncrease()
    {
        SqliteScoreStore s(":memory:");
        const qint64 a = s.addLevelClear(clear(1, 1, 1));
        const qint64 b = s.addLevelClear(clear(1, 2, 2));
        QVERIFY(a > 0);
        QVERIFY(b > a);
    }
    void levelBest_isPerLevel()
    {
        SqliteScoreStore s(":memory:");
        s.addLevelClear(clear(2, 100, 5000000));
        s.addLevelClear(clear(2, 300, 9000000));
        s.addLevelClear(clear(2, 200, 2123456));
        s.addLevelClear(clear(5, 999, 1));
        const LevelBest b = s.levelBest(2);
        QCOMPARE(b.bestScore, 300);
        QCOMPARE(b.bestTimeMicros, qint64(2123456));
        QCOMPARE(b.clears, 3);
        QCOMPARE(s.levelBest(5).bestScore, 999);
        QCOMPARE(s.levelBest(5).clears, 1);
        const LevelBest none = s.levelBest(9);
        QCOMPARE(none.bestScore, 0);
        QCOMPARE(none.bestTimeMicros, qint64(0));
        QCOMPARE(none.clears, 0);
    }
    void topLevelScores_order_and_limit()
    {
        SqliteScoreStore s(":memory:");
        const qint64 a = s.addLevelClear(clear(4, 200, 9000));
        const qint64 b = s.addLevelClear(clear(4, 500, 8000));
        const qint64 c = s.addLevelClear(clear(4, 200, 7000));   // ties with a on score, faster
        const qint64 d = s.addLevelClear(clear(4, 200, 7000));   // ties with c completely -> id order
        s.addLevelClear(clear(6, 9999, 1));
        const auto top = s.topLevelScores(4, 10);
        QCOMPARE(top.size(), 4);
        QCOMPARE(top[0].id, b);
        QCOMPARE(top[1].id, c);
        QCOMPARE(top[2].id, d);
        QCOMPARE(top[3].id, a);
        QCOMPARE(s.topLevelScores(4, 2).size(), 2);
        QVERIFY(s.topLevelScores(4, 0).isEmpty());
        QVERIFY(s.topLevelScores(4, -1).isEmpty());
    }
    void fastestLevelTimes_order()
    {
        SqliteScoreStore s(":memory:");
        const qint64 a = s.addLevelClear(clear(7, 100, 5000));
        const qint64 b = s.addLevelClear(clear(7, 400, 5000));   // same time, higher score first
        const qint64 c = s.addLevelClear(clear(7, 50, 1234567));
        const qint64 d = s.addLevelClear(clear(7, 10, 4999));
        const auto fast = s.fastestLevelTimes(7, 10);
        QCOMPARE(fast.size(), 4);
        QCOMPARE(fast[0].id, d);
        QCOMPARE(fast[1].id, b);
        QCOMPARE(fast[2].id, a);
        QCOMPARE(fast[3].id, c);
        QCOMPARE(s.fastestLevelTimes(7, 1).size(), 1);
    }
    void runs_order_and_initials()
    {
        SqliteScoreStore s(":memory:");
        const qint64 a = s.addRun(run(1000, 9000000, 3));
        const qint64 b = s.addRun(run(5000, 8000000, 9));
        const qint64 c = s.addRun(run(1000, 7000000, 2));
        QVERIFY(a > 0 && b > a && c > b);
        auto top = s.topRuns(10);
        QCOMPARE(top.size(), 3);
        QCOMPARE(top[0].id, b);
        QCOMPARE(top[1].id, c);                      // same score as a, but faster
        QCOMPARE(top[2].id, a);
        QCOMPARE(top[0].level, 9);
        QCOMPARE(top[0].initials, QString());
        QVERIFY(s.setRunInitials(b, "BNY"));
        QCOMPARE(s.topRuns(1)[0].initials, QStringLiteral("BNY"));
        QVERIFY(!s.setRunInitials(424242, "ABC"));
        QCOMPARE(s.topRuns(2).size(), 2);
        QVERIFY(s.topRuns(0).isEmpty());
    }
    void persistsAcrossInstances_andCreatesDirectories()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("a/b/scores.sqlite");
        qint64 id = 0;
        {
            SqliteScoreStore s(path);
            QVERIFY(s.isOpen());
            id = s.addLevelClear(clear(8, 777, 31415926, "PIE"));
            s.addRun(run(4321, 99999999, 8, "PIE"));
        }
        QVERIFY(QFile::exists(path));
        SqliteScoreStore t(path);
        QVERIFY(t.isOpen());
        const auto got = t.topLevelScores(8, 5);
        QCOMPARE(got.size(), 1);
        QCOMPARE(got[0].id, id);
        QCOMPARE(got[0].timeMicros, qint64(31415926));
        QCOMPARE(t.topRuns(5).size(), 1);
        QCOMPARE(t.topRuns(5)[0].score, 4321);
        const qint64 next = t.addLevelClear(clear(8, 1, 1));
        QVERIFY(next > id);
    }
    void instancesAreIndependent()
    {
        SqliteScoreStore a(":memory:");
        SqliteScoreStore b(":memory:");
        a.addLevelClear(clear(1, 10, 10));
        QCOMPARE(a.levelBest(1).clears, 1);
        QCOMPARE(b.levelBest(1).clears, 0);
    }
    void unwritablePath_isHarmless()
    {
        SqliteScoreStore s("/proc/nonexistent/dir/scores.sqlite");
        QVERIFY(!s.isOpen());
        QCOMPARE(s.addLevelClear(clear(1, 1, 1)), qint64(0));
        QCOMPARE(s.addRun(run(1, 1)), qint64(0));
        QVERIFY(!s.setRunInitials(1, "ABC"));
        QVERIFY(s.topRuns(5).isEmpty());
        QVERIFY(s.topLevelScores(1, 5).isEmpty());
        QCOMPARE(s.levelBest(1).clears, 0);
    }
};

QTEST_GUILESS_MAIN(TstScoreStore)
#include "tst_score_store.moc"
