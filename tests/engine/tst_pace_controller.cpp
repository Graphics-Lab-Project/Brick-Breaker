// Acceptance tests for task "Pace controller". READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "PaceController.h"

using namespace BB;

class TstPaceController : public QObject {
    Q_OBJECT
private slots:
    void startsSlow()
    {
        PaceController p;
        p.resetForLevel();
        QCOMPARE(p.speedState(), SpeedState::Slow);
        QCOMPARE(p.ballSpeed(), 180.0);
        QCOMPARE(p.paddleHits(), 0);
        QCOMPARE(p.boardOffsetRows(), 0);
    }
    void fastOnFiftiethHit()
    {
        PaceController p;
        for (int i = 0; i < 49; ++i) {
            const PaddleHitOutcome o = p.onPaddleHit();
            QVERIFY(!o.becameFast);
            QVERIFY(!o.descended);
        }
        QCOMPARE(p.speedState(), SpeedState::Slow);
        const PaddleHitOutcome o = p.onPaddleHit();
        QVERIFY(o.becameFast);
        QVERIFY(!o.descended);          // the hit that switches to Fast does not descend
        QCOMPARE(p.speedState(), SpeedState::Fast);
        QCOMPARE(p.ballSpeed(), 260.0);
        QCOMPARE(p.paddleHits(), 50);
    }
    void descendsWhileFast_upToFour()
    {
        PaceController p;
        for (int i = 0; i < 50; ++i)
            p.onPaddleHit();
        for (int i = 1; i <= 4; ++i) {
            const PaddleHitOutcome o = p.onPaddleHit();
            QVERIFY(o.descended);
            QVERIFY(!o.becameFast);
            QCOMPARE(p.boardOffsetRows(), i);
        }
        const PaddleHitOutcome o = p.onPaddleHit();
        QVERIFY(!o.descended);
        QCOMPARE(p.boardOffsetRows(), 4);
    }
    void resetForLife_keepsOffset()
    {
        PaceController p;
        for (int i = 0; i < 52; ++i)
            p.onPaddleHit();
        QCOMPARE(p.boardOffsetRows(), 2);
        p.resetForLife();
        QCOMPARE(p.speedState(), SpeedState::Slow);
        QCOMPARE(p.paddleHits(), 0);
        QCOMPARE(p.boardOffsetRows(), 2);
        QCOMPARE(p.ballSpeed(), 180.0);
    }
    void resetForLevel_clearsOffset()
    {
        PaceController p;
        for (int i = 0; i < 52; ++i)
            p.onPaddleHit();
        p.resetForLevel();
        QCOMPARE(p.boardOffsetRows(), 0);
        QCOMPARE(p.speedState(), SpeedState::Slow);
        QCOMPARE(p.paddleHits(), 0);
    }
};

QTEST_APPLESS_MAIN(TstPaceController)
#include "tst_pace_controller.moc"
