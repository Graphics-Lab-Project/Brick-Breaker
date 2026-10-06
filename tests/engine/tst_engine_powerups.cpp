// Acceptance tests for task "Engine power-ups" (GameEngine_powerups.cpp). READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "GameEngine.h"
#include <QSignalSpy>

using namespace BB;
using TestUtil::len;

namespace {
void advance(GameEngine &e, double seconds)
{
    const int frames = int(std::lround(seconds * 60.0));
    for (int i = 0; i < frames; ++i)
        e.tick(1.0 / 60.0);
}
// Ball parked moving up-left in the top-left area, away from everything used below.
const Ball kParked{{30, 200}, {0, 0}};
const QString kFar = QStringLiteral("1......");      // top-left breakable brick, never hit here
const QString kEmpty = QStringLiteral(".......");
QStringList rowsWith(int row, const QString &content, const QString &row0 = kFar)
{
    QStringList r{row0};
    while (r.size() < row)
        r << kEmpty;
    r << content;
    return r;
}
void setup(GameEngine &e, const QStringList &rows, qreal paddleX = 168)
{
    e.debugLoadLevelRows(rows);
    e.debugSetPaddleX(paddleX);
    e.debugSetBalls({kParked});
    e.debugSetState(GameState::Playing);
}
struct Script {
    QVector<int> values;
    int i = 0;
    RandFn fn() { return [this](int) { return i < values.size() ? values[i++] : 1; }; }
};
}

class TstEnginePowerUps : public QObject {
    Q_OBJECT
private slots:
    void spawn_onBrickBroken()
    {
        GameEngine e;
        setup(e, rowsWith(5, "...1..."));
        Script s{{0, 2}};
        e.debugSetRandom(s.fn());
        QSignalSpy spawned(&e, &GameEngine::capsuleSpawned);
        e.debugNotifyBrickBroken(5, 3);
        QCOMPARE(spawned.count(), 1);
        QCOMPARE(spawned[0][0].toInt(), int(CapsuleType::Long));
        QCOMPARE(spawned[0][1].toDouble(), 158.0);   // cell centre (168, 132) - (10, 5)
        QCOMPARE(spawned[0][2].toDouble(), 127.0);
        QCOMPARE(e.debugCapsules().size(), 1);
        advance(e, 1.0 / 60.0);
        QCOMPARE(e.capsules()->rowCount(), 1);
    }
    void spawn_respectsBoardOffset()
    {
        GameEngine e;
        setup(e, rowsWith(5, "...1..."));
        for (int i = 0; i < 51; ++i)
            e.debugPace().onPaddleHit();             // offset 1
        e.debugSyncPace();
        Script s{{0, 0}};
        e.debugSetRandom(s.fn());
        QSignalSpy spawned(&e, &GameEngine::capsuleSpawned);
        e.debugNotifyBrickBroken(5, 3);
        QCOMPARE(spawned.count(), 1);
        QCOMPARE(spawned[0][2].toDouble(), 151.0);
    }
    void noSpawn_whenRollFails()
    {
        GameEngine e;
        setup(e, rowsWith(5, "...1..."));
        Script s{{4}};
        e.debugSetRandom(s.fn());
        QSignalSpy spawned(&e, &GameEngine::capsuleSpawned);
        e.debugNotifyBrickBroken(5, 3);
        QCOMPARE(spawned.count(), 0);
        QVERIFY(e.debugCapsules().isEmpty());
    }
    void capsule_falls()
    {
        GameEngine e;
        setup(e, {kFar});
        e.debugAddCapsule(Capsule{CapsuleType::Gun, {250, 100}});
        advance(e, 0.5);
        BB_REQUIRE_INDEX(e.debugCapsules(), 0);
        BB_NEAR(e.debugCapsules()[0].pos.y, 140.0, 0.01);
    }
    void capsule_frozenWhilePaused()
    {
        GameEngine e;
        setup(e, {kFar});
        e.debugAddCapsule(Capsule{CapsuleType::Gun, {250, 100}});
        e.debugSetState(GameState::Paused);
        advance(e, 0.5);
        BB_REQUIRE_INDEX(e.debugCapsules(), 0);
        QCOMPARE(e.debugCapsules()[0].pos.y, 100.0);
    }
    void catch_long()
    {
        GameEngine e;
        setup(e, {kFar});
        e.debugAddCapsule(Capsule{CapsuleType::Long, {158, 310}});
        QSignalSpy caught(&e, &GameEngine::capsuleCaught);
        QSignalSpy mode(&e, &GameEngine::paddleModeChanged);
        advance(e, 0.2);
        QCOMPARE(caught.count(), 1);
        QCOMPARE(caught[0][0].toInt(), int(CapsuleType::Long));
        QCOMPARE(e.score(), 50);
        QCOMPARE(e.paddleWidth(), 96);
        QCOMPARE(e.paddleMode(), int(PaddleMode::Long));
        QCOMPARE(mode.count(), 1);
        QVERIFY(e.debugCapsules().isEmpty());
        QCOMPARE(e.capsules()->rowCount(), 0);
    }
    void catch_life()
    {
        GameEngine e;
        setup(e, {kFar});
        e.debugAddCapsule(Capsule{CapsuleType::Life, {158, 310}});
        QSignalSpy gained(&e, &GameEngine::lifeGained);
        advance(e, 0.2);
        QCOMPARE(e.lives(), 4);
        QCOMPARE(gained.count(), 1);
    }
    void catch_gun()
    {
        GameEngine e;
        setup(e, {kFar});
        e.debugAddCapsule(Capsule{CapsuleType::Gun, {158, 310}});
        advance(e, 0.2);
        QCOMPARE(e.paddleMode(), int(PaddleMode::Gun));
        QCOMPARE(e.gunAmmo(), 3);
    }
    void catch_multi()
    {
        GameEngine e;
        setup(e, {kFar});
        e.debugSetBalls({Ball{{100, 200}, {0, -180}}});
        e.debugAddCapsule(Capsule{CapsuleType::Multi, {158, 310}});
        QSignalSpy multi(&e, &GameEngine::multiBallActivated);
        advance(e, 0.2);
        QCOMPARE(multi.count(), 1);
        QCOMPARE(e.ballCount(), 4);
        const QVector<Ball> balls = e.debugBalls();
        for (const Ball &b : balls) {
            QVERIFY(b.vel.y < 0);
            BB_NEAR(len(b.vel.x, b.vel.y), 180.0, 1e-6);
        }
        // -50 and +50 degree balls split from the same point: mirror images in x, same y
        BB_NEAR(balls[0].pos.y, balls[3].pos.y, 1e-6);
        BB_NEAR(balls[0].pos.x + balls[3].pos.x, 2 * 100.0, 1e-6);
    }
    void capsule_lost()
    {
        GameEngine e;
        setup(e, {kFar});
        e.debugAddCapsule(Capsule{CapsuleType::Life, {20, 330}});
        QSignalSpy lost(&e, &GameEngine::capsuleLost);
        advance(e, 0.2);
        QCOMPARE(lost.count(), 1);
        QVERIFY(e.debugCapsules().isEmpty());
    }
    void gun_fire()
    {
        GameEngine e;
        setup(e, {kFar});
        e.debugSetPower(PaddleMode::Gun, 3, 3);
        QSignalSpy fired(&e, &GameEngine::projectileFired);
        QSignalSpy ammo(&e, &GameEngine::gunAmmoChanged);
        e.debugFire();
        QCOMPARE(fired.count(), 1);
        QCOMPARE(fired[0][0].toInt(), int(ProjectileKind::Bullet));
        QCOMPARE(fired[0][1].toDouble(), 167.0);
        QCOMPARE(fired[0][2].toDouble(), 312.0);
        QCOMPARE(e.gunAmmo(), 2);
        QCOMPARE(ammo.count(), 1);
        QCOMPARE(e.debugProjectiles().size(), 1);
    }
    void gun_lastShotEndsMode()
    {
        GameEngine e;
        setup(e, {kFar});
        e.debugSetPower(PaddleMode::Gun, 1, 3);
        e.debugFire();
        QCOMPARE(e.gunAmmo(), 0);
        QCOMPARE(e.paddleMode(), int(PaddleMode::Normal));
    }
    void bullet_destroysSilver()
    {
        GameEngine e;
        setup(e, rowsWith(5, "...S..."));
        e.debugSetPower(PaddleMode::Gun, 3, 3);
        QSignalSpy broken(&e, &GameEngine::brickBroken);
        e.debugFire();
        advance(e, 0.6);
        QVERIFY(!e.debugGrid().isAlive(5, 3));
        QCOMPARE(broken.count(), 1);
        QCOMPARE(broken[0][0].toInt(), 5);
        QCOMPARE(broken[0][1].toInt(), 3);
        QCOMPARE(e.score(), 50);
        QVERIFY(e.debugProjectiles().isEmpty());
    }
    void bullet_killsMultiHitBrickOutright()
    {
        GameEngine e;
        setup(e, rowsWith(5, "...3..."));
        e.debugSetPower(PaddleMode::Gun, 3, 3);
        e.debugFire();
        advance(e, 0.6);
        QVERIFY(!e.debugGrid().isAlive(5, 3));
        QCOMPARE(e.score(), 50);
    }
    void bullet_hitsLowestBrickOnly()
    {
        GameEngine e;
        QStringList rows = rowsWith(5, "...1...");
        rows[2] = "...1...";
        setup(e, rows);
        e.debugSetPower(PaddleMode::Gun, 3, 3);
        e.debugFire();
        advance(e, 0.6);
        QVERIFY(!e.debugGrid().isAlive(5, 3));
        QVERIFY(e.debugGrid().isAlive(2, 3));
        QVERIFY(e.debugProjectiles().isEmpty());
    }
    void laser_firesTwoBoltsWithCooldown()
    {
        GameEngine e;
        setup(e, {kFar});
        e.debugSetPower(PaddleMode::Laser, 0, 3);
        QSignalSpy fired(&e, &GameEngine::projectileFired);
        e.debugFire();
        QCOMPARE(fired.count(), 2);
        QCOMPARE(fired[0][0].toInt(), int(ProjectileKind::Laser));
        e.debugFire();
        QCOMPARE(fired.count(), 2);
        advance(e, 0.3);
        e.debugFire();
        QCOMPARE(fired.count(), 4);
    }
    void laser_oneHitEach()
    {
        GameEngine e;
        setup(e, rowsWith(5, "2222222"));
        e.debugSetPower(PaddleMode::Laser, 0, 3);
        QSignalSpy hit(&e, &GameEngine::brickHit);
        e.debugFire();                       // bolts at x 137 (col 2) and 197 (col 4)
        advance(e, 0.6);
        QCOMPARE(e.debugGrid().cell(5, 2).hitsLeft, 1);
        QCOMPARE(e.debugGrid().cell(5, 4).hitsLeft, 1);
        QCOMPARE(e.debugGrid().cell(5, 3).hitsLeft, 2);
        QCOMPARE(e.score(), 20);
        QCOMPARE(hit.count(), 2);
        QVERIFY(e.debugProjectiles().isEmpty());
    }
    void laser_cannotDamageSilver()
    {
        GameEngine e;
        setup(e, rowsWith(5, "SSSSSSS"));
        e.debugSetPower(PaddleMode::Laser, 0, 3);
        e.debugFire();
        advance(e, 0.6);
        QVERIFY(e.debugGrid().isAlive(5, 2));
        QVERIFY(e.debugGrid().isAlive(5, 4));
        QCOMPARE(e.score(), 0);
        QVERIFY(e.debugProjectiles().isEmpty());
    }
    void laser_breakCanDropCapsule()
    {
        GameEngine e;
        setup(e, rowsWith(5, "..1.1.."));
        Script s{{0, 0, 3}};
        e.debugSetRandom(s.fn());
        e.debugSetPower(PaddleMode::Laser, 0, 3);
        QSignalSpy spawned(&e, &GameEngine::capsuleSpawned);
        e.debugFire();
        advance(e, 0.6);
        QCOMPARE(spawned.count(), 1);
    }
    void lastBrickByBullet_clearsLevel()
    {
        GameEngine e;
        setup(e, rowsWith(5, "...1...", QStringLiteral("S......")));
        e.debugSetPower(PaddleMode::Gun, 3, 3);
        QSignalSpy cleared(&e, &GameEngine::levelCleared);
        e.debugFire();
        advance(e, 0.6);
        QCOMPARE(cleared.count(), 1);
        QCOMPARE(e.gameState(), int(GameState::LevelCleared));
    }
};

QTEST_GUILESS_MAIN(TstEnginePowerUps)
#include "tst_engine_powerups.moc"
