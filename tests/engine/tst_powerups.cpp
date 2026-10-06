// Acceptance tests for task "Power-up rules". READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "PowerUps.h"

using namespace BB;

class TstPowerUps : public QObject {
    Q_OBJECT
private slots:
    void life_addsLifeAndEndsPowers()
    {
        PowerState s{PaddleMode::Gun, 2, 3};
        const CapsuleEffect e = PowerUps::apply(s, CapsuleType::Life);
        QCOMPARE(s.lives, 4);
        QCOMPARE(s.mode, PaddleMode::Normal);
        QCOMPARE(s.gunAmmo, 0);
        QVERIFY(e.lifeGained);
        QVERIFY(e.modeChanged);
        QVERIFY(!e.multiBall);
    }
    void life_cappedAtMax()
    {
        PowerState s{PaddleMode::Normal, 0, 9};
        const CapsuleEffect e = PowerUps::apply(s, CapsuleType::Life);
        QCOMPARE(s.lives, 9);
        QVERIFY(!e.lifeGained);
        QVERIFY(!e.modeChanged);
    }
    void multi_onlyFlags()
    {
        PowerState s{PaddleMode::Long, 0, 2};
        const CapsuleEffect e = PowerUps::apply(s, CapsuleType::Multi);
        QVERIFY(e.multiBall);
        QVERIFY(!e.modeChanged);
        QCOMPARE(s.mode, PaddleMode::Long);
        QCOMPARE(s.lives, 2);
    }
    void long_widens()
    {
        PowerState s;
        const CapsuleEffect e = PowerUps::apply(s, CapsuleType::Long);
        QCOMPARE(s.mode, PaddleMode::Long);
        QCOMPARE(s.paddleWidth(), 96);
        QVERIFY(e.modeChanged);
    }
    void gun_setsAmmo()
    {
        PowerState s{PaddleMode::Long, 0, 3};
        PowerUps::apply(s, CapsuleType::Gun);
        QCOMPARE(s.mode, PaddleMode::Gun);
        QCOMPARE(s.gunAmmo, 3);
        QCOMPARE(s.paddleWidth(), 64);
    }
    void gun_refillsWithoutModeChange()
    {
        PowerState s{PaddleMode::Gun, 1, 3};
        const CapsuleEffect e = PowerUps::apply(s, CapsuleType::Gun);
        QCOMPARE(s.gunAmmo, 3);
        QVERIFY(!e.modeChanged);
    }
    void laser_replacesGun()
    {
        PowerState s{PaddleMode::Gun, 2, 3};
        const CapsuleEffect e = PowerUps::apply(s, CapsuleType::Laser);
        QCOMPARE(s.mode, PaddleMode::Laser);
        QCOMPARE(s.gunAmmo, 0);
        QVERIFY(e.modeChanged);
    }
    void long_replacesLaser()
    {
        PowerState s{PaddleMode::Laser, 0, 3};
        PowerUps::apply(s, CapsuleType::Long);
        QCOMPARE(s.mode, PaddleMode::Long);
    }
    void resetForLifeLost()
    {
        PowerState s{PaddleMode::Gun, 2, 2};
        QVERIFY(PowerUps::resetForLifeLost(s));
        QCOMPARE(s.mode, PaddleMode::Normal);
        QCOMPARE(s.gunAmmo, 0);
        QCOMPARE(s.lives, 2);
        QVERIFY(!PowerUps::resetForLifeLost(s));
    }
};

QTEST_APPLESS_MAIN(TstPowerUps)
#include "tst_powerups.moc"
