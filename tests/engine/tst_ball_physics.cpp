// Acceptance tests for task "Ball physics". READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "BallPhysics.h"
#include "Constants.h"

using namespace BB;
using namespace BB::BallPhysics;
using TestUtil::deg;

class TstBallPhysics : public QObject {
    Q_OBJECT
private slots:
    void paddleBounce_centreIsStraightUp()
    {
        const Vec2 v = paddleBounce(168, 168, 64, 180);
        BB_NEAR(v.x, 0.0, 1e-9);
        BB_NEAR(v.y, -180.0, 1e-9);
    }
    void paddleBounce_rightEdgeIs60Degrees()
    {
        const Vec2 v = paddleBounce(200, 168, 64, 180);
        BB_NEAR(v.x, 180.0 * std::sin(deg(60)), 1e-6);
        BB_NEAR(v.y, -180.0 * std::cos(deg(60)), 1e-6);
    }
    void paddleBounce_leftHalfwayIs30Degrees()
    {
        const Vec2 v = paddleBounce(152, 168, 64, 260);
        BB_NEAR(v.x, -260.0 * std::sin(deg(30)), 1e-6);
        BB_NEAR(v.y, -260.0 * std::cos(deg(30)), 1e-6);
    }
    void paddleBounce_beyondEdgeIsClamped()
    {
        const Vec2 v = paddleBounce(250, 168, 64, 180);
        BB_NEAR(v.x, 180.0 * std::sin(deg(60)), 1e-6);
        BB_NEAR(v.y, -180.0 * std::cos(deg(60)), 1e-6);
    }
    void paddleBounce_longPaddleUsesItsWidth()
    {
        const Vec2 v = paddleBounce(168 + 24, 168, 96, 180);   // half of 48
        BB_NEAR(v.x, 180.0 * std::sin(deg(30)), 1e-6);
    }
    void paddleBounce_zeroWidthIsStraightUp()
    {
        const Vec2 v = paddleBounce(200, 168, 0, 180);
        BB_NEAR(v.x, 0.0, 1e-9);
        BB_NEAR(v.y, -180.0, 1e-9);
    }
    void launch_rightByDefault()
    {
        const Vec2 r = launchVelocity(0, 180);
        BB_NEAR(r.x, 180.0 * std::sin(deg(30)), 1e-6);
        BB_NEAR(r.y, -180.0 * std::cos(deg(30)), 1e-6);
        const Vec2 r2 = launchVelocity(1, 180);
        BB_NEAR(r2.x, r.x, 1e-9);
    }
    void launch_left()
    {
        const Vec2 l = launchVelocity(-1, 180);
        BB_NEAR(l.x, -180.0 * std::sin(deg(30)), 1e-6);
        QVERIFY(l.y < 0);
    }
    void multiBall_fourUpwardAtAngles()
    {
        const QVector<Vec2> v = multiBallVelocities(180);
        QCOMPARE(v.size(), 4);
        const double angles[4] = {-50, -20, 20, 50};
        for (int i = 0; i < 4; ++i) {
            BB_NEAR(v[i].x, 180.0 * std::sin(deg(angles[i])), 1e-6);
            BB_NEAR(v[i].y, -180.0 * std::cos(deg(angles[i])), 1e-6);
        }
    }
    void walls_left()
    {
        Ball b{{2, 100}, {-100, 50}};
        QVERIFY(bounceWalls(b));
        QCOMPARE(b.pos.x, 3.0);
        QCOMPARE(b.vel.x, 100.0);
        QCOMPARE(b.vel.y, 50.0);
    }
    void walls_right()
    {
        Ball b{{335, 100}, {100, 50}};
        QVERIFY(bounceWalls(b));
        QCOMPARE(b.pos.x, 333.0);
        QCOMPARE(b.vel.x, -100.0);
    }
    void walls_top()
    {
        Ball b{{100, 1}, {10, -90}};
        QVERIFY(bounceWalls(b));
        QCOMPARE(b.pos.y, 3.0);
        QCOMPARE(b.vel.y, 90.0);
    }
    void walls_corner()
    {
        Ball b{{1, 1}, {-10, -10}};
        QVERIFY(bounceWalls(b));
        QCOMPARE(b.pos.x, 3.0);
        QCOMPARE(b.pos.y, 3.0);
        QCOMPARE(b.vel.x, 10.0);
        QCOMPARE(b.vel.y, 10.0);
    }
    void walls_alreadyMovingAway_keepsDirection()
    {
        // touching the left wall but already moving right: velocity must still point right
        Ball b{{2, 100}, {100, 0}};
        bounceWalls(b);
        QCOMPARE(b.vel.x, 100.0);
    }
    void walls_insideNoHit()
    {
        Ball b{{100, 100}, {10, 10}};
        QVERIFY(!bounceWalls(b));
        QCOMPARE(b.pos.x, 100.0);
        QCOMPARE(b.vel.x, 10.0);
    }
    void walls_bottomIsOpen()
    {
        Ball b{{100, 335}, {0, 100}};
        QVERIFY(!bounceWalls(b));
        QCOMPARE(b.vel.y, 100.0);
    }
    void lost()
    {
        QVERIFY(!isLost(Ball{{100, 330}, {}}));
        QVERIFY(!isLost(Ball{{100, 339}, {}}));   // top edge exactly 336
        QVERIFY(isLost(Ball{{100, 339.5}, {}}));
    }
    void resting()
    {
        const Ball b = restingBall(150);
        QCOMPARE(b.pos.x, 150.0);
        QCOMPARE(b.pos.y, 319.0);
        QCOMPARE(b.vel.x, 0.0);
        QCOMPARE(b.vel.y, 0.0);
    }
    void withSpeed_rescales()
    {
        const Vec2 v = withSpeed(Vec2{3, -4}, 260);
        BB_NEAR(v.x, 156.0, 1e-9);
        BB_NEAR(v.y, -208.0, 1e-9);
    }
    void withSpeed_zeroGoesUp()
    {
        const Vec2 v = withSpeed(Vec2{0, 0}, 180);
        BB_NEAR(v.x, 0.0, 1e-9);
        BB_NEAR(v.y, -180.0, 1e-9);
    }
};

QTEST_APPLESS_MAIN(TstBallPhysics)
#include "tst_ball_physics.moc"
