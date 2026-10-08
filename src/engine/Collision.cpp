// OWNER: task "Collision helpers". Implementation of Collision.h.
#include "Collision.h"
#include "Constants.h"

namespace BB::Collision {

Rect cellRect(int row, int col, int boardOffsetRows)
{
    return Rect{col * K::CellW, (row + boardOffsetRows) * K::CellH, K::CellW, K::CellH};
}

QVector<QPoint> cellsOverlapping(const Rect &r, int boardOffsetRows)
{
    QVector<QPoint> cells;
    for (int row = 0; row < K::Rows; ++row) {
        for (int col = 0; col < K::Cols; ++col) {
            if (cellRect(row, col, boardOffsetRows).intersects(r))
                cells.append(QPoint(col, row));
        }
    }
    return cells;
}

HitAxis ballHitAxis(const Rect &prevBall, const Rect &curBall, const Rect &target)
{
    if (!curBall.intersects(target))
        return HitAxis::None;

    const bool prevOverlapX = prevBall.right() > target.left() && prevBall.left() < target.right();
    const bool prevOverlapY = prevBall.bottom() > target.top() && prevBall.top() < target.bottom();

    if (prevOverlapX)            // top/bottom face, or already inside
        return HitAxis::Y;
    if (prevOverlapY)            // side face
        return HitAxis::X;
    return HitAxis::Both;        // corner
}

Vec2 reflect(const Vec2 &vel, HitAxis axis)
{
    Vec2 out = vel;
    if (axis == HitAxis::X || axis == HitAxis::Both)
        out.x = -vel.x;
    if (axis == HitAxis::Y || axis == HitAxis::Both)
        out.y = -vel.y;
    return out;
}

Rect ballRect(const Vec2 &centre)
{
    return Rect{centre.x - K::BallHalf, centre.y - K::BallHalf, K::BallSize, K::BallSize};
}

} // namespace BB::Collision
