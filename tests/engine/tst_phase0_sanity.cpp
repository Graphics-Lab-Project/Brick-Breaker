// OWNER: Phase 0. Read-only. Passes at Phase 0: checks the frozen scaffolding itself.
#include "TestUtil.h"
#include "Constants.h"
#include "GameEngine.h"
#include "Rng.h"
#include "Types.h"

using namespace BB;

class TstPhase0Sanity : public QObject {
    Q_OBJECT
private slots:
    void constants()
    {
        QCOMPARE(K::Cols * K::CellW, K::FieldW);
        QCOMPARE(K::Rows * K::CellH, K::FieldH);
        QCOMPARE(K::PaddleSpeeds[K::DefaultPaddleSpeedSetting - 1], 240.0);
        QCOMPARE(K::BallRestY, K::PaddleTopY - K::BallHalf);
    }
    void rectIntersectsIsStrict()
    {
        const Rect a{0, 0, 10, 10};
        QVERIFY(a.intersects(Rect{5, 5, 10, 10}));
        QVERIFY(!a.intersects(Rect{10, 0, 10, 10}));   // touching edge only
        QVERIFY(!a.intersects(Rect{0, 10, 10, 10}));
    }
    void rngIsDeterministic()
    {
        Rng a(42), b(42);
        for (int i = 0; i < 100; ++i) {
            const int x = a.nextInt(6);
            QCOMPARE(x, b.nextInt(6));
            QVERIFY(x >= 0 && x < 6);
        }
    }
    void engineConstructsAndTicks()
    {
        GameEngine e;
        QCOMPARE(e.gameState(), int(GameState::Menu));
        QVERIFY(e.bricks() && e.balls() && e.capsules() && e.projectiles());
        e.tick(1.0);       // clamped, must not crash
        e.tick(-1.0);
        QCOMPARE(e.storagePath(), QString());
    }
};

QTEST_GUILESS_MAIN(TstPhase0Sanity)
#include "tst_phase0_sanity.moc"
