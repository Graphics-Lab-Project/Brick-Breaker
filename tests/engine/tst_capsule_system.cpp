// Acceptance tests for task "Capsule system". READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "CapsuleSystem.h"
#include "Constants.h"

using namespace BB;

namespace {
// Returns the scripted values in order; records every n it was called with.
struct Script {
    QVector<int> values;
    QVector<int> asked;
    int i = 0;
    RandFn fn()
    {
        return [this](int n) {
            asked << n;
            return i < values.size() ? values[i++] : 0;
        };
    }
};
}

class TstCapsuleSystem : public QObject {
    Q_OBJECT
private slots:
    void spawn_whenRollIsZero()
    {
        Script s{{0, 2}};
        CapsuleSystem cs(s.fn());
        const auto c = cs.maybeSpawn(Vec2{168, 60});
        QVERIFY(c.has_value());
        QCOMPARE(c->type, CapsuleType::Long);
        QCOMPARE(c->pos.x, 158.0);
        QCOMPARE(c->pos.y, 55.0);
        QCOMPARE(cs.capsules().size(), 1);
        QCOMPARE(s.asked, (QVector<int>{6, 5}));
    }
    void noSpawn_whenRollNonZero()
    {
        Script s{{3}};
        CapsuleSystem cs(s.fn());
        QVERIFY(!cs.maybeSpawn(Vec2{100, 100}).has_value());
        QVERIFY(cs.capsules().isEmpty());
        QCOMPARE(s.asked, (QVector<int>{6}));   // type is not rolled
    }
    void noRandom_neverSpawns()
    {
        CapsuleSystem cs;
        QVERIFY(!cs.maybeSpawn(Vec2{100, 100}).has_value());
    }
    void allTypes()
    {
        for (int t = 0; t < 5; ++t) {
            Script s{{0, t}};
            CapsuleSystem cs(s.fn());
            QCOMPARE(int(cs.maybeSpawn(Vec2{50, 50})->type), t);
        }
    }
    void falls()
    {
        CapsuleSystem cs;
        cs.add(Capsule{CapsuleType::Gun, {100, 50}});
        for (int i = 0; i < 120; ++i)
            cs.step(K::FixedDt);
        BB_REQUIRE_INDEX(cs.capsules(), 0);
        BB_NEAR(cs.capsules()[0].pos.y, 130.0, 1e-6);
        BB_REQUIRE_INDEX(cs.capsules(), 0);
        QCOMPARE(cs.capsules()[0].pos.x, 100.0);
    }
    void caughtByPaddle()
    {
        CapsuleSystem cs;
        cs.add(Capsule{CapsuleType::Life, {150, 315}});    // overlaps paddle at y 322..328
        cs.add(Capsule{CapsuleType::Gun, {10, 315}});      // far left, not over the paddle
        cs.add(Capsule{CapsuleType::Laser, {160, 318}});
        const Rect paddle{136, 322, 64, 6};
        const QVector<CapsuleType> got = cs.collectCaught(paddle);
        QCOMPARE(got, (QVector<CapsuleType>{CapsuleType::Life, CapsuleType::Laser}));
        QCOMPARE(cs.capsules().size(), 1);
        BB_REQUIRE_INDEX(cs.capsules(), 0);
        QCOMPARE(cs.capsules()[0].type, CapsuleType::Gun);
    }
    void touchingIsNotCaught()
    {
        CapsuleSystem cs;
        cs.add(Capsule{CapsuleType::Life, {150, 312}});    // bottom == 322 == paddle top
        QVERIFY(cs.collectCaught(Rect{136, 322, 64, 6}).isEmpty());
    }
    void lostBelowField()
    {
        CapsuleSystem cs;
        cs.add(Capsule{CapsuleType::Life, {50, 337}});
        cs.add(Capsule{CapsuleType::Multi, {50, 336}});    // top exactly at FieldH: not yet lost
        cs.add(Capsule{CapsuleType::Long, {50, 200}});
        QCOMPARE(cs.removeLost(), 1);
        QCOMPARE(cs.capsules().size(), 2);
    }
    void clearAndRect()
    {
        CapsuleSystem cs;
        cs.add(Capsule{CapsuleType::Life, {1, 2}});
        BB_REQUIRE_INDEX(cs.capsules(), 0);
        const Rect r = CapsuleSystem::rectOf(cs.capsules()[0]);
        QCOMPARE(r.w, 20.0);
        QCOMPARE(r.h, 10.0);
        cs.clear();
        QVERIFY(cs.capsules().isEmpty());
    }
    void deterministicWithRng()
    {
        Rng a(7), b(7);
        CapsuleSystem x(a.fn()), y(b.fn());
        for (int i = 0; i < 200; ++i) {
            const auto cx = x.maybeSpawn(Vec2{100, 100});
            const auto cy = y.maybeSpawn(Vec2{100, 100});
            QCOMPARE(cx.has_value(), cy.has_value());
            if (cx)
                QCOMPARE(cx->type, cy->type);
        }
        QVERIFY(x.capsules().size() > 10);              // roughly 1 in 6 of 200
        QVERIFY(x.capsules().size() < 70);
    }
};

QTEST_APPLESS_MAIN(TstCapsuleSystem)
#include "tst_capsule_system.moc"
