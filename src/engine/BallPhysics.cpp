// OWNER: task "Ball physics". Phase 0 stub: compiles, behaviour missing.
#include "BallPhysics.h"
#include "Constants.h"

#include <cmath>

using namespace BB::K;

namespace BB::BallPhysics {

namespace {

qreal degToRad(qreal deg)
{
    return deg * std::acos(-1.0) / 180.0;
}

// Velocity of the given speed at `angleDeg` from vertical (positive = right), upward.
Vec2 upwardAt(qreal angleDeg, qreal speed)
{
    const qreal a = degToRad(angleDeg);
    return { speed * std::sin(a), -speed * std::cos(a) };
}

} // namespace

Vec2 paddleBounce(qreal ballX, qreal paddleX, qreal paddleWidth, qreal speed)
{
    qreal t = 0.0;
    if (paddleWidth > 0.0) {
        t = (ballX - paddleX) / (paddleWidth / 2.0);
        if (t < -1.0) t = -1.0;
        if (t > 1.0) t = 1.0;
    }
    return upwardAt(t * MaxBounceAngleDeg, speed);
}

Vec2 launchVelocity(int dir, qreal speed)
{
    const qreal angle = dir < 0 ? -LaunchAngleDeg : LaunchAngleDeg;
    return upwardAt(angle, speed);
}

QVector<Vec2> multiBallVelocities(qreal speed)
{
    QVector<Vec2> out;
    out.reserve(4);
    for (qreal angle : MultiAnglesDeg)
        out.append(upwardAt(angle, speed));
    return out;
}

bool bounceWalls(Ball &b)
{
    bool hit = false;
    const qreal minX = BallHalf;
    const qreal maxX = FieldW - BallHalf;
    const qreal minY = BallHalf;

    if (b.pos.x < minX) {
        b.pos.x = minX;
        b.vel.x = std::fabs(b.vel.x);
        hit = true;
    } else if (b.pos.x > maxX) {
        b.pos.x = maxX;
        b.vel.x = -std::fabs(b.vel.x);
        hit = true;
    }

    if (b.pos.y < minY) {
        b.pos.y = minY;
        b.vel.y = std::fabs(b.vel.y);
        hit = true;
    }
    return hit;
}

bool isLost(const Ball &b)
{
    return (b.pos.y - BallHalf) > FieldH;
}

Ball restingBall(qreal paddleX)
{
    return Ball{ Vec2{ paddleX, BallRestY }, Vec2{ 0.0, 0.0 } };
}

Vec2 withSpeed(const Vec2 &v, qreal speed)
{
    const qreal len = std::hypot(v.x, v.y);
    if (len <= 0.0)
        return { 0.0, -speed };
    const qreal k = speed / len;
    return { v.x * k, v.y * k };
}

} // namespace BB::BallPhysics
