// Acceptance tests for task "Engine ball step" (GameEngine_balls.cpp). READ-ONLY for agents (AGENTS.md R4).
#include "TestUtil.h"
#include "GameEngine.h"
#include <QSignalSpy>

using namespace BB;
using TestUtil::deg;
using TestUtil::len;

namespace {
void advance(GameEngine &e, double seconds)
{
    const int frames = int(std::lround(seconds * 60.0));
    for (int i = 0; i < frames; ++i)
        e.tick(1.0 / 60.0);
}
void setup(GameEngine &e, const QStringList &rows, const QVector<Ball> &balls, qreal paddleX = 168)
{
    e.debugLoadLevelRows(rows);
    e.debugSetPaddleX(paddleX);
    e.debugSetBalls(balls);
    e.debugSetState(GameState::Playing);
}
// A breakable brick in the top-right corner so a level never clears by accident.
const QString kFar = QStringLiteral("......1");
const QString kEmpty = QStringLiteral(".......");
QStringList rowsWithRow5(const QString &row5)
{
    return {kFar, kEmpty, kEmpty, kEmpty, kEmpty, row5};
}
QVariant brickRole(GameEngine &e, int row, int col, const char *role)
{
    BrickModel *m = e.bricks();
    const int r = m->roleNames().key(QByteArray(role), -1);
    return m->data(m->index(row * 7 + col, 0), r);
}
}

class TstEngineBalls : public QObject {
    Q_OBJECT
private slots:
    void movesWithVelocity()
    {
        GameEngine e;
        setup(e, {kFar}, {Ball{{100, 200}, {0, -180}}});
        advance(e, 0.1);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        BB_NEAR(e.debugBalls()[0].pos.y, 182.0, 0.01);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QCOMPARE(e.debugBalls()[0].pos.x, 100.0);
    }
    void dtSpikeIsClamped()
    {
        GameEngine e;
        setup(e, {kFar}, {Ball{{100, 200}, {0, -180}}});
        e.tick(1.0);                        // clamped to 0.05 s -> 9 px
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        BB_NEAR(e.debugBalls()[0].pos.y, 191.0, 0.01);
    }
    void bouncesOffLeftWall()
    {
        GameEngine e;
        setup(e, {kFar}, {Ball{{10, 200}, {-180, 0}}});
        advance(e, 0.1);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        const Ball b = e.debugBalls()[0];
        QVERIFY(b.vel.x > 0);
        QVERIFY(b.pos.x >= 3.0);
    }
    void bouncesOffTopWall()
    {
        GameEngine e;
        setup(e, {kFar}, {Ball{{100, 10}, {0, -180}}});
        advance(e, 0.1);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QVERIFY(e.debugBalls()[0].vel.y > 0);
    }
    void paddle_centreBounce()
    {
        GameEngine e;
        setup(e, {kFar}, {Ball{{168, 310}, {0, 180}}});
        advance(e, 0.1);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        const Ball b = e.debugBalls()[0];
        QVERIFY(b.vel.y < 0);
        BB_NEAR(b.vel.x, 0.0, 1e-6);
        BB_NEAR(len(b.vel.x, b.vel.y), 180.0, 1e-6);
        QVERIFY(b.pos.y <= 319.0);
        QCOMPARE(e.debugPace().paddleHits(), 1);
    }
    void paddle_edgeBounceIs60Degrees()
    {
        GameEngine e;
        setup(e, {kFar}, {Ball{{200, 310}, {0, 180}}});
        advance(e, 0.1);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        const Ball b = e.debugBalls()[0];
        BB_NEAR(b.vel.x, 180.0 * std::sin(deg(60)), 1e-3);
        BB_NEAR(b.vel.y, -180.0 * std::cos(deg(60)), 1e-3);
    }
    void paddle_missedBallIsLost()
    {
        GameEngine e;
        QSignalSpy lost(&e, &GameEngine::lifeLost);
        setup(e, {kFar}, {Ball{{20, 310}, {0, 180}}});
        advance(e, 0.3);
        QCOMPARE(lost.count(), 1);
    }
    void brick_hitFromBelow()
    {
        GameEngine e;
        setup(e, rowsWithRow5(QStringLiteral("...2...")), {Ball{{168, 160}, {0, -180}}});
        QSignalSpy hit(&e, &GameEngine::brickHit);
        QSignalSpy broken(&e, &GameEngine::brickBroken);
        advance(e, 0.15);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QVERIFY(e.debugBalls()[0].vel.y > 0);
        QCOMPARE(e.debugGrid().cell(5, 3).hitsLeft, 1);
        QCOMPARE(e.score(), 10);
        QCOMPARE(hit.count(), 1);
        QCOMPARE(hit[0][0].toInt(), 5);
        QCOMPARE(hit[0][1].toInt(), 3);
        QCOMPARE(hit[0][2].toInt(), 1);
        QCOMPARE(hit[0][3].toBool(), false);
        QCOMPARE(broken.count(), 0);
        QCOMPARE(brickRole(e, 5, 3, "hitsLeft").toInt(), 1);
    }
    void brick_breaks()
    {
        GameEngine e;
        setup(e, rowsWithRow5(QStringLiteral("...1...")), {Ball{{168, 160}, {0, -180}}});
        QSignalSpy broken(&e, &GameEngine::brickBroken);
        advance(e, 0.15);
        QCOMPARE(broken.count(), 1);
        QCOMPARE(broken[0][0].toInt(), 5);
        QCOMPARE(broken[0][1].toInt(), 3);
        QCOMPARE(broken[0][2].toInt(), 1);
        QVERIFY(!e.debugGrid().isAlive(5, 3));
        QCOMPARE(e.score(), 10);
        QCOMPARE(brickRole(e, 5, 3, "alive").toBool(), false);
    }
    void brick_sideHitFlipsX()
    {
        GameEngine e;
        setup(e, rowsWithRow5(QStringLiteral("...3...")), {Ball{{130, 132}, {180, 0}}});
        advance(e, 0.1);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        const Ball b = e.debugBalls()[0];
        QVERIFY(b.vel.x < 0);
        QCOMPARE(b.vel.y, 0.0);
        QCOMPARE(e.debugGrid().cell(5, 3).hitsLeft, 2);
    }
    void brick_silverReflectsWithoutScore()
    {
        GameEngine e;
        setup(e, rowsWithRow5(QStringLiteral("...S...")), {Ball{{168, 160}, {0, -180}}});
        QSignalSpy hit(&e, &GameEngine::brickHit);
        advance(e, 0.15);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QVERIFY(e.debugBalls()[0].vel.y > 0);
        QCOMPARE(e.score(), 0);
        QVERIFY(e.debugGrid().isAlive(5, 3));
        QCOMPARE(hit.count(), 1);
        QCOMPARE(hit[0][3].toBool(), true);
    }
    void brick_noTunnellingAtFastSpeed()
    {
        GameEngine e;
        setup(e, rowsWithRow5(QStringLiteral("...1...")), {Ball{{168, 300}, {0, -260}}});
        for (int i = 0; i < 20; ++i)
            e.tick(0.05);                   // large frames, still sub-stepped
        QVERIFY(!e.debugGrid().isAlive(5, 3));
    }
    void lostBall_losesLifeAndReady()
    {
        GameEngine e;
        QSignalSpy lost(&e, &GameEngine::lifeLost);
        setup(e, {kFar}, {Ball{{100, 330}, {0, 180}}});
        advance(e, 0.1);
        QCOMPARE(lost.count(), 1);
        QCOMPARE(e.lives(), 2);
        QCOMPARE(e.gameState(), int(GameState::Ready));
        QCOMPARE(e.ballCount(), 1);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QCOMPARE(e.debugBalls()[0].pos.x, 168.0);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QCOMPARE(e.debugBalls()[0].pos.y, 319.0);
    }
    void lostOneOfTwo_keepsLife()
    {
        GameEngine e;
        QSignalSpy lost(&e, &GameEngine::lifeLost);
        setup(e, {kFar}, {Ball{{100, 330}, {0, 180}}, Ball{{100, 200}, {0, -180}}});
        advance(e, 0.1);
        QCOMPARE(lost.count(), 0);
        QCOMPARE(e.lives(), 3);
        QCOMPARE(e.ballCount(), 1);
        QCOMPARE(e.gameState(), int(GameState::Playing));
    }
    void lastLife_gameOver()
    {
        GameEngine e;
        QSignalSpy over(&e, &GameEngine::gameOver);
        setup(e, {kFar}, {Ball{{100, 330}, {0, 180}}});
        e.debugSetPower(PaddleMode::Normal, 0, 1);
        advance(e, 0.1);
        QCOMPARE(over.count(), 1);
        QCOMPARE(e.lives(), 0);
        QCOMPARE(e.gameState(), int(GameState::GameOver));
        QCOMPARE(e.ballCount(), 0);
    }
    void lifeLost_resetsPowers()
    {
        GameEngine e;
        setup(e, {kFar}, {Ball{{100, 330}, {0, 180}}});
        e.debugSetPower(PaddleMode::Long, 0, 3);
        QCOMPARE(e.paddleWidth(), 96);
        advance(e, 0.1);
        QCOMPARE(e.paddleWidth(), 64);
        QCOMPARE(e.paddleMode(), 0);
    }
    void levelCleared_whenLastBreakableBrickBreaks()
    {
        GameEngine e;
        // only silver remains after the '1' breaks
        setup(e, {QStringLiteral("S......"), kEmpty, kEmpty, kEmpty, kEmpty, QStringLiteral("...1...")},
              {Ball{{168, 160}, {0, -180}}});
        QSignalSpy cleared(&e, &GameEngine::levelCleared);
        advance(e, 0.15);
        QCOMPARE(cleared.count(), 1);
        QCOMPARE(e.gameState(), int(GameState::LevelCleared));
    }
    void fast_afterFiftiethPaddleHit()
    {
        GameEngine e;
        setup(e, {kFar}, {Ball{{168, 310}, {0, 180}}});
        for (int i = 0; i < 49; ++i)
            e.debugPace().onPaddleHit();
        QSignalSpy speed(&e, &GameEngine::speedStateChanged);
        advance(e, 0.1);
        QCOMPARE(e.speedState(), int(SpeedState::Fast));
        QCOMPARE(speed.count(), 1);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        const Ball b = e.debugBalls()[0];
        BB_NEAR(len(b.vel.x, b.vel.y), 260.0, 1e-6);
    }
    void fast_rescalesOtherBalls()
    {
        GameEngine e;
        setup(e, {kFar}, {Ball{{168, 310}, {0, 180}}, Ball{{60, 200}, {0, -180}}});
        for (int i = 0; i < 49; ++i)
            e.debugPace().onPaddleHit();
        advance(e, 0.1);
        BB_REQUIRE_INDEX(e.debugBalls(), 1);
        const Ball other = e.debugBalls()[1];
        BB_NEAR(len(other.vel.x, other.vel.y), 260.0, 1e-6);
    }
    void descent_whileFast()
    {
        GameEngine e;
        setup(e, {kFar}, {Ball{{168, 310}, {0, 180}}});
        for (int i = 0; i < 50; ++i)
            e.debugPace().onPaddleHit();
        e.debugSyncPace();
        QSignalSpy shifted(&e, &GameEngine::boardShifted);
        advance(e, 0.1);
        QCOMPARE(shifted.count(), 1);
        QCOMPARE(shifted[0][0].toInt(), 1);
        QCOMPARE(e.boardOffsetRows(), 1);
    }
    void descent_movesCollisions()
    {
        GameEngine e;
        setup(e, {QStringLiteral("...2...")}, {Ball{{168, 60}, {0, -260}}});
        for (int i = 0; i < 51; ++i)
            e.debugPace().onPaddleHit();   // Fast, offset 1 -> row 0 drawn at y 24..48
        e.debugSyncPace();
        QCOMPARE(e.boardOffsetRows(), 1);
        advance(e, 0.06);
        BB_REQUIRE_INDEX(e.debugBalls(), 0);
        QVERIFY(e.debugBalls()[0].vel.y > 0);
        QCOMPARE(e.debugGrid().cell(0, 3).hitsLeft, 1);
    }
};

QTEST_GUILESS_MAIN(TstEngineBalls)
#include "tst_engine_balls.moc"
