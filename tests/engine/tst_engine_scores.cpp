// Acceptance tests for task "Engine score persistence" (GameEngine.cpp). READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "GameEngine.h"
#include "SqliteScoreStore.h"
#include <QSignalSpy>
#include <QTemporaryDir>

using namespace BB;

namespace {
void advance(GameEngine &e, double seconds)
{
    const int frames = int(std::lround(seconds * 60.0));
    for (int i = 0; i < frames; ++i)
        e.tick(1.0 / 60.0);
}

// Records what the engine hands to the store.
class FakeStore : public ScoreStore {
public:
    bool isOpen() const override { return true; }
    qint64 addLevelClear(const LevelClearRecord &r) override { clears.append(r); return clears.size(); }
    qint64 addRun(const RunRecord &r) override { runs.append(r); return 100 + runs.size(); }
    bool setRunInitials(qint64 id, const QString &i) override { initialsFor.insert(id, i); return true; }
    QVector<RunRecord> topRuns(int) const override { return {}; }
    QVector<LevelClearRecord> topLevelScores(int, int) const override { return {}; }
    QVector<LevelClearRecord> fastestLevelTimes(int, int) const override { return {}; }
    LevelBest levelBest(int) const override { return {}; }
    QVector<LevelClearRecord> clears;
    QVector<RunRecord> runs;
    QHash<qint64, QString> initialsFor;
};
}

class TstEngineScores : public QObject {
    Q_OBJECT
private slots:
    void withoutStore_timersStillCount_queriesAreEmpty()
    {
        GameEngine e;
        QVERIFY(e.scoreStore() == nullptr);
        e.startGame();
        e.launchOrFire();
        advance(e, 0.25);
        QCOMPARE(e.runTimeMicros(), qint64(250000));
        e.debugEnterLevelCleared();                       // no store: must not crash
        QCOMPARE(e.levelBest(1).value("clears").toInt(), 0);
        QCOMPARE(e.levelBest(1).value("bestTimeMicros").toLongLong(), qint64(0));
        QVERIFY(e.levelScores(1).isEmpty());
        QVERIFY(e.runScores().isEmpty());
        QCOMPARE(e.levelBests().size(), K::LevelCount);
    }
    void playTime_countsOnlyPlayingSteps()
    {
        GameEngine e;
        e.startGame();
        advance(e, 1.0);                                  // Ready: no time
        QCOMPARE(e.runTimeMicros(), qint64(0));
        e.launchOrFire();
        advance(e, 0.25);
        QCOMPARE(e.runTimeMicros(), qint64(250000));
        QCOMPARE(e.levelTimeMicros(), qint64(250000));
        e.togglePause();
        advance(e, 1.0);                                  // Paused: no time
        QCOMPARE(e.runTimeMicros(), qint64(250000));
        e.togglePause();
        advance(e, 0.25);
        QCOMPARE(e.runTimeMicros(), qint64(500000));
        QCOMPARE(e.levelTimeMicros(), qint64(500000));
    }
    void levelClear_isRecordedWithScoreDeltaAndTime()
    {
        QTemporaryDir dir;
        GameEngine e;
        e.setStoragePath(dir.filePath("bb.ini"));
        QVERIFY(e.scoreStore() != nullptr);
        QVERIFY(e.scoreStore()->isOpen());
        QSignalSpy bestsSpy(&e, &GameEngine::levelBestsChanged);
        e.startGame();
        e.launchOrFire();
        advance(e, 0.25);
        e.debugSetScore(250);
        advance(e, 0.25);
        e.debugEnterLevelCleared();
        QVERIFY(bestsSpy.count() >= 1);
        const QVariantMap b = e.levelBest(1);
        QCOMPARE(b.value("bestScore").toInt(), 250);
        QCOMPARE(b.value("bestTimeMicros").toLongLong(), qint64(500000));
        QCOMPARE(b.value("clears").toInt(), 1);
        const QVariantMap viaList = e.levelBests().value(0).toMap();
        QCOMPARE(viaList.value("bestScore").toInt(), 250);
        QCOMPARE(viaList.value("bestTimeMicros").toLongLong(), qint64(500000));
        const QVariantList scores = e.levelScores(1, 5);
        QCOMPARE(scores.size(), 1);
        const QVariantMap first = scores.value(0).toMap();
        QCOMPARE(first.value("level").toInt(), 1);
        QCOMPARE(first.value("round").toInt(), 1);
        QCOMPARE(first.value("score").toInt(), 250);
        QCOMPARE(first.value("timeMicros").toLongLong(), qint64(500000));
        QVERIFY(QDateTime::fromString(first.value("at").toString(), Qt::ISODateWithMs).isValid());
        QCOMPARE(e.levelTimes(1, 5).size(), 1);

        // next level: level timer restarts, score delta is relative to the level start, run timer keeps going
        advance(e, 1.6);
        QCOMPARE(e.level(), 2);
        QCOMPARE(e.levelTimeMicros(), qint64(0));
        QCOMPARE(e.runTimeMicros(), qint64(500000));
        e.launchOrFire();
        advance(e, 0.25);
        e.debugSetScore(400);
        e.debugEnterLevelCleared();
        const QVariantMap b2 = e.levelBest(2);
        QCOMPARE(b2.value("bestScore").toInt(), 150);
        QCOMPARE(b2.value("bestTimeMicros").toLongLong(), qint64(250000));
        QCOMPARE(e.levelBest(1).value("bestScore").toInt(), 250);
    }
    void gameOver_recordsRun_andInitialsUpdateIt()
    {
        QTemporaryDir dir;
        GameEngine e;
        e.setStoragePath(dir.filePath("bb.ini"));
        e.startGame();
        e.launchOrFire();
        advance(e, 0.25);
        e.debugSetScore(1240);
        e.debugEnterGameOver();
        QVariantList runs = e.runScores(5);
        QCOMPARE(runs.size(), 1);
        QCOMPARE(runs[0].toMap().value("score").toInt(), 1240);
        QCOMPARE(runs[0].toMap().value("level").toInt(), 1);
        QCOMPARE(runs[0].toMap().value("timeMicros").toLongLong(), qint64(250000));
        QCOMPARE(runs[0].toMap().value("initials").toString(), QString());
        e.submitInitials("bny");
        runs = e.runScores(5);
        QCOMPARE(runs.size(), 1);
        QCOMPARE(runs[0].toMap().value("initials").toString(), QStringLiteral("BNY"));
    }
    void newRun_resetsRunTimer_andRunTimeSpansLevels()
    {
        QTemporaryDir dir;
        GameEngine e;
        e.setStoragePath(dir.filePath("bb.ini"));
        e.setUnlockAll(true);
        e.startLevel(3);
        e.launchOrFire();
        advance(e, 0.25);
        e.debugEnterLevelCleared();
        advance(e, 1.6);
        QCOMPARE(e.level(), 4);
        e.launchOrFire();
        advance(e, 0.25);
        e.debugEnterGameOver();
        const QVariantMap run = e.runScores(1).value(0).toMap();
        QCOMPARE(run.value("level").toInt(), 4);
        QCOMPARE(run.value("timeMicros").toLongLong(), qint64(500000));
        e.startLevel(1);
        QCOMPARE(e.runTimeMicros(), qint64(0));
        QCOMPARE(e.levelTimeMicros(), qint64(0));
        e.submitInitials("ZZZ");                          // a new run is not game over: no effect on the old record
        QCOMPARE(e.runScores(1).value(0).toMap().value("initials").toString(), QString());
    }
    void persistsBetweenSessions()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("bb.ini");
        {
            GameEngine e;
            e.setStoragePath(path);
            e.startGame();
            e.launchOrFire();
            advance(e, 0.25);
            e.debugSetScore(300);
            e.debugEnterLevelCleared();
            advance(e, 1.6);
            e.launchOrFire();
            advance(e, 0.25);
            e.debugSetScore(900);
            e.debugEnterGameOver();
            e.submitInitials("AAA");
        }
        GameEngine f;
        QSignalSpy bests(&f, &GameEngine::levelBestsChanged);
        f.setStoragePath(path);
        QVERIFY(bests.count() >= 1);
        QCOMPARE(f.levelBest(1).value("bestScore").toInt(), 300);
        QCOMPARE(f.levelBest(1).value("bestTimeMicros").toLongLong(), qint64(250000));
        QCOMPARE(f.levelBests().value(0).toMap().value("bestScore").toInt(), 300);
        QCOMPARE(f.runScores(5).value(0).toMap().value("score").toInt(), 900);
        QCOMPARE(f.runScores(5).value(0).toMap().value("initials").toString(), QStringLiteral("AAA"));
    }
    void injectedStore_receivesRecords()
    {
        GameEngine e;
        auto fake = std::make_unique<FakeStore>();
        FakeStore *s = fake.get();
        QSignalSpy bests(&e, &GameEngine::levelBestsChanged);
        e.setScoreStore(std::move(fake));
        QCOMPARE(e.scoreStore(), static_cast<ScoreStore *>(s));
        QVERIFY(bests.count() >= 1);
        e.startGame();
        e.launchOrFire();
        advance(e, 0.25);
        e.debugSetScore(80);
        e.debugEnterLevelCleared();
        QCOMPARE(s->clears.size(), 1);
        QCOMPARE(s->clears[0].level, 1);
        QCOMPARE(s->clears[0].round, 1);
        QCOMPARE(s->clears[0].score, 80);
        QCOMPARE(s->clears[0].timeMicros, qint64(250000));
        QVERIFY(s->clears[0].atUtc.isValid());
        advance(e, 1.6);
        e.debugEnterGameOver();
        QCOMPARE(s->runs.size(), 1);
        QCOMPARE(s->runs[0].score, 80);
        QCOMPARE(s->runs[0].timeMicros, qint64(250000));
        e.submitInitials("QQQ");
        QCOMPARE(s->initialsFor.value(101), QStringLiteral("QQQ"));
        e.setScoreStore(nullptr);                         // detaching is allowed
        QVERIFY(e.scoreStore() == nullptr);
    }
    void outOfRangeLevel_isEmpty()
    {
        QTemporaryDir dir;
        GameEngine e;
        e.setStoragePath(dir.filePath("bb.ini"));
        QCOMPARE(e.levelBest(0).value("clears").toInt(), 0);
        QCOMPARE(e.levelBest(K::LevelCount + 1).value("clears").toInt(), 0);
        QVERIFY(e.levelScores(0).isEmpty());
        QVERIFY(e.levelTimes(K::LevelCount + 1).isEmpty());
    }
};

QTEST_GUILESS_MAIN(TstEngineScores)
#include "tst_engine_scores.moc"
