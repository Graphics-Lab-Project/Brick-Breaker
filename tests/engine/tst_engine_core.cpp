// Acceptance tests for task "Engine core" (GameEngine.cpp). READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "GameEngine.h"
#include <QSignalSpy>
#include <QTemporaryDir>

using namespace BB;
using TestUtil::deg;

namespace {
void advance(GameEngine &e, double seconds)
{
    const int frames = int(std::lround(seconds * 60.0));
    for (int i = 0; i < frames; ++i)
        e.tick(1.0 / 60.0);
}
}

class TstEngineCore : public QObject {
    Q_OBJECT
private slots:
    void initialState()
    {
        GameEngine e;
        QCOMPARE(e.gameState(), int(GameState::Menu));
        QCOMPARE(e.lives(), 3);
        QCOMPARE(e.score(), 0);
        QCOMPARE(e.level(), 1);
        QCOMPARE(e.round(), 1);
        QCOMPARE(e.paddleSpeed(), 3);
        QCOMPARE(e.acceleration(), false);
        QCOMPARE(e.ballCount(), 0);
        QCOMPARE(e.paddleX(), 168.0);
        QCOMPARE(e.paddleWidth(), 64);
        QVERIFY(e.highScores().isEmpty());
    }
    void startGame_entersReadyWithLevel1()
    {
        GameEngine e;
        QSignalSpy state(&e, &GameEngine::gameStateChanged);
        e.startGame();
        QCOMPARE(e.gameState(), int(GameState::Ready));
        QVERIFY(state.count() >= 1);
        QCOMPARE(e.ballCount(), 1);
        QCOMPARE(e.balls()->rowCount(), 1);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        const Ball b = e.debugBalls()[0];
        QCOMPARE(b.pos.x, 168.0);
        QCOMPARE(b.pos.y, 319.0);
        QCOMPARE(e.bricks()->rowCount(), 98);
        QCOMPARE(e.debugGrid().aliveCount(), 24);
        QCOMPARE(e.lives(), 3);
        QCOMPARE(e.level(), 1);
    }
    void startGame_ignoredWhilePlaying()
    {
        GameEngine e;
        e.startGame();
        e.launchOrFire();
        e.debugSetScore(500);
        e.startGame();
        QCOMPARE(e.gameState(), int(GameState::Playing));
        QCOMPARE(e.score(), 500);
    }
    void startGame_fromGameOverResetsEverything()
    {
        GameEngine e;
        e.startGame();
        e.debugSetScore(900);
        e.debugSetPower(PaddleMode::Gun, 2, 1);
        e.debugSetLevel(4);
        e.debugEnterGameOver();
        e.startGame();
        QCOMPARE(e.gameState(), int(GameState::Ready));
        QCOMPARE(e.score(), 0);
        QCOMPARE(e.lives(), 3);
        QCOMPARE(e.level(), 1);
        QCOMPARE(e.round(), 1);
        QCOMPARE(e.paddleMode(), 0);
        QCOMPARE(e.gunAmmo(), 0);
        QCOMPARE(e.paddleWidth(), 64);
    }
    void readyBallFollowsPaddle()
    {
        GameEngine e;
        e.startGame();
        e.moveRight(true);
        QCOMPARE(e.inputDir(), 1);
        advance(e, 0.5);
        BB_NEAR(e.paddleX(), 288.0, 0.5);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        const Ball b = e.debugBalls()[0];
        BB_NEAR(b.pos.x, e.paddleX(), 1e-9);
        QCOMPARE(b.pos.y, 319.0);
        e.moveRight(false);
        QCOMPARE(e.inputDir(), 0);
    }
    void inputDir_bothKeys()
    {
        GameEngine e;
        QSignalSpy spy(&e, &GameEngine::inputDirChanged);
        e.moveLeft(true);
        QCOMPARE(e.inputDir(), -1);
        e.moveRight(true);
        QCOMPARE(e.inputDir(), 0);
        e.moveRight(false);
        QCOMPARE(e.inputDir(), -1);
        e.moveLeft(false);
        QCOMPARE(e.inputDir(), 0);
        QVERIFY(spy.count() >= 3);
    }
    void paddleIdleOutsideReadyPlaying()
    {
        GameEngine e;                   // Menu
        e.moveRight(true);
        advance(e, 0.5);
        QCOMPARE(e.paddleX(), 168.0);
        e.setPointerX(50);
        QCOMPARE(e.paddleX(), 168.0);
    }
    void pointerMovesPaddleAndBall()
    {
        GameEngine e;
        e.startGame();
        e.setPointerX(100);
        QCOMPARE(e.paddleX(), 100.0);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QCOMPARE(e.debugBalls()[0].pos.x, 100.0);
    }
    void launch_rightWhenStill()
    {
        GameEngine e;
        e.startGame();
        e.launchOrFire();
        QCOMPARE(e.gameState(), int(GameState::Playing));
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        const Ball b = e.debugBalls()[0];
        BB_NEAR(b.vel.x, 180.0 * std::sin(deg(30)), 1e-6);
        BB_NEAR(b.vel.y, -180.0 * std::cos(deg(30)), 1e-6);
    }
    void launch_leftWhenMovingLeft()
    {
        GameEngine e;
        e.startGame();
        e.moveLeft(true);
        e.launchOrFire();
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QVERIFY(e.debugBalls()[0].vel.x < 0);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QVERIFY(e.debugBalls()[0].vel.y < 0);
    }
    void launch_followsPointerDirection()
    {
        GameEngine e;
        e.startGame();
        e.setPointerX(100);             // moved left, no step in between
        e.launchOrFire();
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QVERIFY(e.debugBalls()[0].vel.x < 0);
    }
    void launch_ignoredInMenu()
    {
        GameEngine e;
        e.launchOrFire();
        QCOMPARE(e.gameState(), int(GameState::Menu));
    }
    void pause_freezesEverything()
    {
        GameEngine e;
        e.startGame();
        e.launchOrFire();
        e.togglePause();
        QCOMPARE(e.gameState(), int(GameState::Paused));
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        const Ball before = e.debugBalls()[0];
        e.moveRight(true);
        advance(e, 0.5);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QCOMPARE(e.debugBalls()[0].pos.x, before.pos.x);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QCOMPARE(e.debugBalls()[0].pos.y, before.pos.y);
        QCOMPARE(e.paddleX(), 168.0);
        e.togglePause();
        QCOMPARE(e.gameState(), int(GameState::Playing));
    }
    void pause_fromReadyReturnsToReady()
    {
        GameEngine e;
        e.startGame();
        e.togglePause();
        QCOMPARE(e.gameState(), int(GameState::Paused));
        e.togglePause();
        QCOMPARE(e.gameState(), int(GameState::Ready));
    }
    void quitToMenu_clearsBalls()
    {
        GameEngine e;
        e.startGame();
        e.launchOrFire();
        e.quitToMenu();
        QCOMPARE(e.gameState(), int(GameState::Menu));
        QCOMPARE(e.ballCount(), 0);
        QCOMPARE(e.balls()->rowCount(), 0);
    }
    void paddleSpeed_clampedAndNotified()
    {
        GameEngine e;
        QSignalSpy spy(&e, &GameEngine::paddleSpeedChanged);
        e.setPaddleSpeed(9);
        QCOMPARE(e.paddleSpeed(), 5);
        QCOMPARE(spy.count(), 1);
        e.setPaddleSpeed(5);
        QCOMPARE(spy.count(), 1);       // unchanged -> no signal
        e.setPaddleSpeed(0);
        QCOMPARE(e.paddleSpeed(), 1);
    }
    void paddleSpeed_affectsMovement()
    {
        GameEngine e;
        e.setPaddleSpeed(1);            // 120 px/s
        e.startGame();
        e.moveRight(true);
        advance(e, 0.5);
        BB_NEAR(e.paddleX(), 228.0, 0.5);
    }
    void acceleration_notified()
    {
        GameEngine e;
        QSignalSpy spy(&e, &GameEngine::accelerationChanged);
        e.setAcceleration(true);
        QCOMPARE(e.acceleration(), true);
        QCOMPARE(spy.count(), 1);
    }
    void settings_persist()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("bb.ini");
        {
            GameEngine e;
            e.setStoragePath(path);
            e.setPaddleSpeed(5);
            e.setAcceleration(true);
        }
        GameEngine f;
        f.setStoragePath(path);
        QCOMPARE(f.paddleSpeed(), 5);
        QCOMPARE(f.acceleration(), true);
    }
    void settings_defaultsWithoutFile()
    {
        QTemporaryDir dir;
        GameEngine e;
        e.setPaddleSpeed(5);
        e.setStoragePath(dir.filePath("missing.ini"));
        QCOMPARE(e.paddleSpeed(), 3);    // reloaded from an empty store -> defaults
        QCOMPARE(e.acceleration(), false);
    }
    void levelCleared_advancesAfterDelay()
    {
        GameEngine e;
        e.startGame();
        e.launchOrFire();
        QSignalSpy levelSpy(&e, &GameEngine::levelChanged);
        e.debugEnterLevelCleared();
        QCOMPARE(e.gameState(), int(GameState::LevelCleared));
        advance(e, 1.4);
        QCOMPARE(e.gameState(), int(GameState::LevelCleared));
        advance(e, 0.2);
        QCOMPARE(e.gameState(), int(GameState::Ready));
        QCOMPARE(e.level(), 2);
        QCOMPARE(levelSpy.count(), 1);
        QCOMPARE(e.debugGrid().aliveCount(), 21);
        QCOMPARE(e.ballCount(), 1);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QCOMPARE(e.debugBalls()[0].pos.y, 319.0);
    }
    void levelCleared_resetsPowersKeepsLives()
    {
        GameEngine e;
        e.startGame();
        e.debugSetPower(PaddleMode::Long, 0, 5);
        e.debugEnterLevelCleared();
        advance(e, 1.6);
        QCOMPARE(e.paddleMode(), 0);
        QCOMPARE(e.paddleWidth(), 64);
        QCOMPARE(e.lives(), 5);
    }
    void level10_loopsAndIncrementsRound()
    {
        GameEngine e;
        e.startGame();
        e.debugSetLevel(10);
        QSignalSpy roundSpy(&e, &GameEngine::roundChanged);
        e.debugEnterLevelCleared();
        advance(e, 1.6);
        QCOMPARE(e.level(), 1);
        QCOMPARE(e.round(), 2);
        QCOMPARE(roundSpy.count(), 1);
        QCOMPARE(e.debugGrid().aliveCount(), 24);
    }
    void highScore_submitAndPersist()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("bb.ini");
        {
            GameEngine e;
            e.setStoragePath(path);
            e.startGame();
            e.debugSetScore(1240);
            e.debugEnterGameOver();
            QVERIFY(e.highScorePending());
            QSignalSpy hs(&e, &GameEngine::highScoresChanged);
            e.submitInitials("bny");
            QVERIFY(!e.highScorePending());
            QVERIFY(hs.count() >= 1);
            const QVariantMap top = e.highScores().value(0).toMap();
            QCOMPARE(top.value("initials").toString(), QStringLiteral("BNY"));
            QCOMPARE(top.value("score").toInt(), 1240);
            QCOMPARE(top.value("level").toInt(), 1);
            QCOMPARE(e.bestScore(), 1240);
        }
        GameEngine f;
        f.setStoragePath(path);
        QCOMPARE(f.highScores().size(), 1);
        QCOMPARE(f.bestScore(), 1240);
    }
    void highScore_invalidInitialsKeepPending()
    {
        GameEngine e;
        e.startGame();
        e.debugSetScore(100);
        e.debugEnterGameOver();
        e.submitInitials("A");
        QVERIFY(e.highScorePending());
        QVERIFY(e.highScores().isEmpty());
    }
    void highScore_ignoredWhenNotPending()
    {
        GameEngine e;
        e.startGame();
        e.submitInitials("ABC");
        QVERIFY(e.highScores().isEmpty());
    }
};

QTEST_GUILESS_MAIN(TstEngineCore)
#include "tst_engine_core.moc"
