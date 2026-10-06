// Acceptance tests for task "Game state machine". READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "GameStateMachine.h"

using namespace BB;
using S = GameState;
using E = GameEvent;

class TstGameStateMachine : public QObject {
    Q_OBJECT
private slots:
    void startsInMenu()
    {
        GameStateMachine m;
        QCOMPARE(m.state(), S::Menu);
    }
    void happyPath()
    {
        GameStateMachine m;
        QVERIFY(m.handle(E::StartGame));
        QCOMPARE(m.state(), S::Ready);
        QVERIFY(m.handle(E::Launch));
        QCOMPARE(m.state(), S::Playing);
        QVERIFY(m.handle(E::BallLost));
        QCOMPARE(m.state(), S::Ready);
        QVERIFY(m.handle(E::Launch));
        QVERIFY(m.handle(E::LevelCleared));
        QCOMPARE(m.state(), S::LevelCleared);
        QVERIFY(m.handle(E::NextLevel));
        QCOMPARE(m.state(), S::Ready);
        QVERIFY(m.handle(E::Launch));
        QVERIFY(m.handle(E::LastBallLost));
        QCOMPARE(m.state(), S::GameOver);
        QVERIFY(m.handle(E::StartGame));
        QCOMPARE(m.state(), S::Ready);
    }
    void pauseRemembersPlaying()
    {
        GameStateMachine m;
        m.forceState(S::Playing);
        QVERIFY(m.handle(E::TogglePause));
        QCOMPARE(m.state(), S::Paused);
        QVERIFY(m.handle(E::TogglePause));
        QCOMPARE(m.state(), S::Playing);
    }
    void pauseRemembersReady()
    {
        GameStateMachine m;
        m.forceState(S::Ready);
        QVERIFY(m.handle(E::TogglePause));
        QVERIFY(m.handle(E::TogglePause));
        QCOMPARE(m.state(), S::Ready);
    }
    void pausedRejectsGameplayEvents()
    {
        GameStateMachine m;
        m.forceState(S::Playing);
        m.handle(E::TogglePause);
        QVERIFY(!m.handle(E::Launch));
        QVERIFY(!m.handle(E::BallLost));
        QVERIFY(!m.handle(E::LevelCleared));
        QCOMPARE(m.state(), S::Paused);
    }
    void invalidTransitions()
    {
        GameStateMachine m;              // Menu
        QVERIFY(!m.handle(E::Launch));
        QVERIFY(!m.handle(E::TogglePause));
        QVERIFY(!m.handle(E::BallLost));
        QVERIFY(!m.handle(E::NextLevel));
        QVERIFY(!m.handle(E::QuitToMenu));
        QCOMPARE(m.state(), S::Menu);
        m.forceState(S::Ready);
        QVERIFY(!m.handle(E::StartGame));
        QVERIFY(!m.handle(E::BallLost));
        QVERIFY(!m.handle(E::LevelCleared));
        m.forceState(S::LevelCleared);
        QVERIFY(!m.handle(E::TogglePause));
        QVERIFY(!m.handle(E::Launch));
        m.forceState(S::GameOver);
        QVERIFY(!m.handle(E::TogglePause));
        QVERIFY(!m.handle(E::NextLevel));
    }
    void quitFromAnywhere()
    {
        const S states[] = {S::Ready, S::Playing, S::Paused, S::LevelCleared, S::GameOver};
        for (S s : states) {
            GameStateMachine m;
            m.forceState(s);
            QVERIFY(m.handle(E::QuitToMenu));
            QCOMPARE(m.state(), S::Menu);
        }
    }
    void reset()
    {
        GameStateMachine m;
        m.forceState(S::Playing);
        m.reset();
        QCOMPARE(m.state(), S::Menu);
    }
};

QTEST_APPLESS_MAIN(TstGameStateMachine)
#include "tst_game_state_machine.moc"
