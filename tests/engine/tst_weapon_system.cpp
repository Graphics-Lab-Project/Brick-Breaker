// Acceptance tests for task "Weapon system". READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "Constants.h"
#include "WeaponSystem.h"

using namespace BB;

class TstWeaponSystem : public QObject {
    Q_OBJECT
private slots:
    void gun_firesOneBullet()
    {
        WeaponSystem w;
        PowerState s{PaddleMode::Gun, 3, 3};
        QCOMPARE(w.fire(s, 168, 64), 1);
        QCOMPARE(s.gunAmmo, 2);
        QCOMPARE(w.projectiles().size(), 1);
        BB_REQUIRE_INDEX(w.projectiles(), 0);
        const Projectile p = w.projectiles()[0];
        QCOMPARE(p.kind, ProjectileKind::Bullet);
        QCOMPARE(p.pos.x, 167.0);
        QCOMPARE(p.pos.y, 312.0);   // 322 - 4 - 6
    }
    void gun_lastShotReturnsToNormal()
    {
        WeaponSystem w;
        PowerState s{PaddleMode::Gun, 1, 3};
        QCOMPARE(w.fire(s, 100, 64), 1);
        QCOMPARE(s.gunAmmo, 0);
        QCOMPARE(s.mode, PaddleMode::Normal);
        QCOMPARE(w.fire(s, 100, 64), 0);
    }
    void gun_noCooldown()
    {
        WeaponSystem w;
        PowerState s{PaddleMode::Gun, 3, 3};
        w.fire(s, 100, 64);
        QCOMPARE(w.fire(s, 100, 64), 1);
        QCOMPARE(w.projectiles().size(), 2);
    }
    void gun_zeroAmmoDoesNothing()
    {
        WeaponSystem w;
        PowerState s{PaddleMode::Gun, 0, 3};
        QCOMPARE(w.fire(s, 100, 64), 0);
        QVERIFY(w.projectiles().isEmpty());
    }
    void laser_firesTwoBolts()
    {
        WeaponSystem w;
        PowerState s{PaddleMode::Laser, 0, 3};
        QCOMPARE(w.fire(s, 168, 64), 2);
        QCOMPARE(w.projectiles().size(), 2);
        BB_REQUIRE_INDEX(w.projectiles(), 0);
        const Projectile a = w.projectiles()[0];
        BB_REQUIRE_INDEX(w.projectiles(), 1);
        const Projectile b = w.projectiles()[1];
        QCOMPARE(a.kind, ProjectileKind::Laser);
        QCOMPARE(b.kind, ProjectileKind::Laser);
        QCOMPARE(a.pos.x, 137.0);   // 168 - 32 + 1
        QCOMPARE(b.pos.x, 197.0);   // 168 + 32 - 1 - 2
        QCOMPARE(a.pos.y, 308.0);   // 322 - 4 - 10
        QCOMPARE(s.mode, PaddleMode::Laser);
        BB_NEAR(w.cooldown(), 0.25, 1e-9);
    }
    void laser_cooldown()
    {
        WeaponSystem w;
        PowerState s{PaddleMode::Laser, 0, 3};
        w.fire(s, 168, 64);
        QCOMPARE(w.fire(s, 168, 64), 0);
        for (int i = 0; i < 29; ++i)            // 0.2417 s
            w.step(K::FixedDt);
        QCOMPARE(w.fire(s, 168, 64), 0);
        w.step(K::FixedDt * 2);                  // past 0.25 s
        QCOMPARE(w.fire(s, 168, 64), 2);
    }
    void normalAndLong_doNothing()
    {
        WeaponSystem w;
        PowerState n{PaddleMode::Normal, 3, 3};
        PowerState l{PaddleMode::Long, 3, 3};
        QCOMPARE(w.fire(n, 168, 64), 0);
        QCOMPARE(w.fire(l, 168, 96), 0);
        QVERIFY(w.projectiles().isEmpty());
    }
    void step_movesUp()
    {
        WeaponSystem w;
        PowerState s{PaddleMode::Gun, 3, 3};
        w.fire(s, 168, 64);
        for (int i = 0; i < 60; ++i)
            w.step(K::FixedDt);                  // 0.5 s * 400 = 200 px
        BB_REQUIRE_INDEX(w.projectiles(), 0);
        BB_NEAR(w.projectiles()[0].pos.y, 112.0, 1e-6);
    }
    void step_removesOffTop()
    {
        WeaponSystem w;
        PowerState s{PaddleMode::Gun, 3, 3};
        w.fire(s, 168, 64);
        for (int i = 0; i < 120; ++i)            // 1 s -> y = -88
            w.step(K::FixedDt);
        QVERIFY(w.projectiles().isEmpty());
    }
    void cooldownNeverNegative()
    {
        WeaponSystem w;
        w.step(1.0);
        QCOMPARE(w.cooldown(), 0.0);
    }
    void removeAndClear()
    {
        WeaponSystem w;
        PowerState s{PaddleMode::Laser, 0, 3};
        w.fire(s, 168, 64);
        w.remove(0);
        QCOMPARE(w.projectiles().size(), 1);
        BB_REQUIRE_INDEX(w.projectiles(), 0);
        QCOMPARE(w.projectiles()[0].pos.x, 197.0);
        w.remove(5);                             // out of range: no-op
        QCOMPARE(w.projectiles().size(), 1);
        w.clear();
        QVERIFY(w.projectiles().isEmpty());
        QCOMPARE(w.cooldown(), 0.0);
    }
    void rectOf()
    {
        const Rect b = WeaponSystem::rectOf(Projectile{ProjectileKind::Bullet, {10, 20}});
        QCOMPARE(b.h, 6.0);
        const Rect l = WeaponSystem::rectOf(Projectile{ProjectileKind::Laser, {10, 20}});
        QCOMPARE(l.h, 10.0);
    }
};

QTEST_APPLESS_MAIN(TstWeaponSystem)
#include "tst_weapon_system.moc"
