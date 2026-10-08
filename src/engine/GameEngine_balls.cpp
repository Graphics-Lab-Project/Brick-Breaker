// OWNER: task "Engine ball step".
#include "GameEngine.h"

namespace BB {

void GameEngine::stepBalls(qreal h)
{
    QVector<Ball> kept;
    kept.reserve(m_balls.size());

    for (int i = 0; i < m_balls.size(); ++i) {
        Ball b = m_balls[i];
        const Vec2 prevPos = b.pos;
        b.pos.x += b.vel.x * h;
        b.pos.y += b.vel.y * h;
        BallPhysics::bounceWalls(b);

        // paddle
        const Rect paddle{m_paddle.x() - m_paddle.width() / 2.0, K::PaddleTopY, qreal(m_paddle.width()),
                          K::PaddleH};
        if (b.vel.y > 0 && Collision::ballRect(b.pos).intersects(paddle)) {
            const PaddleHitOutcome out = m_pace.onPaddleHit();
            b.vel = BallPhysics::paddleBounce(b.pos.x, m_paddle.x(), m_paddle.width(), m_pace.ballSpeed());
            b.pos.y = K::BallRestY;
            syncPace();
            if (out.becameFast) {
                for (int j = 0; j < m_balls.size(); ++j)
                    m_balls[j].vel = BallPhysics::withSpeed(m_balls[j].vel, K::FastSpeed);
                for (int j = 0; j < kept.size(); ++j)
                    kept[j].vel = BallPhysics::withSpeed(kept[j].vel, K::FastSpeed);
                b.vel = BallPhysics::withSpeed(b.vel, K::FastSpeed);
            }
        }

        // bricks: at most one per ball per step
        const Rect prevRect = Collision::ballRect(prevPos);
        const Rect curRect = Collision::ballRect(b.pos);
        const QVector<QPoint> cells = Collision::cellsOverlapping(curRect, m_pace.boardOffsetRows());
        for (const QPoint &p : cells) {
            const int row = p.y();
            const int col = p.x();
            if (!m_grid.isAlive(row, col))
                continue;
            const Rect target = Collision::cellRect(row, col, m_pace.boardOffsetRows());
            const Collision::HitAxis axis = Collision::ballHitAxis(prevRect, curRect, target);
            if (axis == Collision::HitAxis::None)
                continue;
            b.vel = Collision::reflect(b.vel, axis);
            if (axis == Collision::HitAxis::X || axis == Collision::HitAxis::Both)
                b.pos.x = prevPos.x;
            if (axis == Collision::HitAxis::Y || axis == Collision::HitAxis::Both)
                b.pos.y = prevPos.y;

            const BrickHitResult r = m_grid.hit(row, col);
            updateBrick(row, col);
            if (!r.unbreakable)
                addScore(K::BrickHitPoints);
            emit brickHit(row, col, r.hitsLeft, r.unbreakable);
            if (r.broken) {
                emit brickBroken(row, col, r.tier);
                onBrickBroken(row, col);
            }
            break;
        }

        if (!BallPhysics::isLost(b))
            kept.append(b);
    }

    m_balls = kept;
    if (m_balls.isEmpty())
        loseLife();
    else
        checkLevelCleared();
}

} // namespace BB
