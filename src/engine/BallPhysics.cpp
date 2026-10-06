// OWNER: task "Ball physics". Phase 0 stub: compiles, behaviour missing.
#include "BallPhysics.h"
#include "Constants.h"

namespace BB::BallPhysics {

Vec2 paddleBounce(qreal, qreal, qreal, qreal) { return {}; }
Vec2 launchVelocity(int, qreal) { return {}; }
QVector<Vec2> multiBallVelocities(qreal) { return {}; }
bool bounceWalls(Ball &) { return false; }
bool isLost(const Ball &) { return false; }
Ball restingBall(qreal) { return {}; }
Vec2 withSpeed(const Vec2 &v, qreal) { return v; }

} // namespace BB::BallPhysics
