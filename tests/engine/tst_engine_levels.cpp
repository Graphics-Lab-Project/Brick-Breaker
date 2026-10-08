// Acceptance tests for task "Engine level select" (GameEngine.cpp). READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "GameEngine.h"
#include <QSignalSpy>
#include <QTemporaryDir>

using namespace BB;

class TstEngineLevels : public QObject {
    Q_OBJECT
private slots:
    void initialState()
    {
        GameEngine e;
        QCOMPARE(e.unlockedLevel(), 1);
        QCOMPARE(e.hasProgress(), false);
        QCOMPARE(e.unlockAll(), false);
    }
    void startLevel_lockedIsIgnored()
    {
        GameEngine e;
        e.startLevel(2);
        QCOMPARE(e.gameState(), int(GameState::Menu));
        QCOMPARE(e.hasProgress(), false);
    }
    void startLevel_outOfRangeIsIgnored()
    {
        GameEngine e;
        e.setUnlockAll(true);
        e.startLevel(0);
        QCOMPARE(e.gameState(), int(GameState::Menu));
        e.startLevel(11);
        QCOMPARE(e.gameState(), int(GameState::Menu));
        e.startLevel(-3);
        QCOMPARE(e.gameState(), int(GameState::Menu));
    }
    void startLevel1_entersReady_andSetsProgress()
    {
        GameEngine e;
        QSignalSpy progress(&e, &GameEngine::hasProgressChanged);
        e.startLevel(1);
        QCOMPARE(e.gameState(), int(GameState::Ready));
        QCOMPARE(e.level(), 1);
        QCOMPARE(e.hasProgress(), true);
        QCOMPARE(progress.count(), 1);
    }
    void startGame_alsoSetsProgress()
    {
        GameEngine e;
        e.startGame();
        QCOMPARE(e.hasProgress(), true);
    }
    void unlockAll_opensEverything()
    {
        GameEngine e;
        QSignalSpy spy(&e, &GameEngine::unlockedLevelChanged);
        QSignalSpy allSpy(&e, &GameEngine::unlockAllChanged);
        e.setUnlockAll(true);
        QCOMPARE(e.unlockAll(), true);
        QCOMPARE(e.unlockedLevel(), K::LevelCount);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(allSpy.count(), 1);
        e.setUnlockAll(true);                       // no change -> no signal
        QCOMPARE(allSpy.count(), 1);
        e.setUnlockAll(false);
        QCOMPARE(e.unlockedLevel(), 1);
        QCOMPARE(spy.count(), 2);
    }
    void startLevel_loadsThatLevelWithFreshStats()
    {
        GameEngine e;
        e.setUnlockAll(true);
        e.startGame();
        e.debugSetScore(500);
        e.debugEnterGameOver();
        QCOMPARE(e.gameState(), int(GameState::GameOver));
        e.startLevel(7);
        QCOMPARE(e.gameState(), int(GameState::Ready));
        QCOMPARE(e.level(), 7);
        QCOMPARE(e.round(), 1);
        QCOMPARE(e.score(), 0);
        QCOMPARE(e.lives(), 3);
        QCOMPARE(e.ballCount(), 1);
        BrickGrid expected;
        QVERIFY(expected.load(Levels::rows(7)));
        QCOMPARE(e.debugGrid().aliveCount(), expected.aliveCount());
    }
    void startLevel_ignoredWhilePlaying()
    {
        GameEngine e;
        e.setUnlockAll(true);
        e.startLevel(3);
        e.launchOrFire();
        QCOMPARE(e.gameState(), int(GameState::Playing));
        e.startLevel(5);
        QCOMPARE(e.gameState(), int(GameState::Playing));
        QCOMPARE(e.level(), 3);
    }
    void clearingALevel_unlocksTheNext()
    {
        GameEngine e;
        e.startGame();
        QSignalSpy spy(&e, &GameEngine::unlockedLevelChanged);
        e.debugEnterLevelCleared();
        QCOMPARE(e.unlockedLevel(), 2);
        QCOMPARE(spy.count(), 1);
        e.quitToMenu();                             // still unlocked after leaving during the banner
        QCOMPARE(e.unlockedLevel(), 2);
        e.startLevel(2);
        QCOMPARE(e.gameState(), int(GameState::Ready));
        QCOMPARE(e.level(), 2);
    }
    void clearingAnEarlierLevel_neverLowersProgress()
    {
        GameEngine e;
        e.setUnlockAll(true);
        e.startLevel(4);
        e.debugEnterLevelCleared();                 // unlocks 5 for real
        e.setUnlockAll(false);
        QCOMPARE(e.unlockedLevel(), 5);
        e.quitToMenu();
        e.startLevel(1);
        e.debugEnterLevelCleared();                 // clears level 1 again
        QCOMPARE(e.unlockedLevel(), 5);
    }
    void clearingLastLevel_capsAtLevelCount()
    {
        GameEngine e;
        e.setUnlockAll(true);
        e.startLevel(K::LevelCount);
        e.debugEnterLevelCleared();
        e.setUnlockAll(false);
        QCOMPARE(e.unlockedLevel(), K::LevelCount);
    }
    void progressPersists_unlockAllDoesNot()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("bb.ini");
        {
            GameEngine e;
            e.setStoragePath(path);
            QCOMPARE(e.hasProgress(), false);
            e.startGame();
            e.debugEnterLevelCleared();
            e.setUnlockAll(true);                   // dev switch must not be saved
            QCOMPARE(e.unlockedLevel(), K::LevelCount);
        }
        GameEngine f;
        QSignalSpy spy(&f, &GameEngine::unlockedLevelChanged);
        f.setStoragePath(path);
        QCOMPARE(f.unlockedLevel(), 2);
        QCOMPARE(f.hasProgress(), true);
        QCOMPARE(f.unlockAll(), false);
        QVERIFY(spy.count() >= 1);
    }
    void noStoragePath_keepsNothing()
    {
        {
            GameEngine e;
            e.startGame();
            e.debugEnterLevelCleared();
        }
        GameEngine f;
        QCOMPARE(f.unlockedLevel(), 1);
        QCOMPARE(f.hasProgress(), false);
    }
};

QTEST_GUILESS_MAIN(TstEngineLevels)
#include "tst_engine_levels.moc"
